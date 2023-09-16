#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

typedef enum Conclusion {
    POR_REALIZARSE, CONCLUIDA
} Conclusion;

typedef struct Fecha {
    char dia[3];
    char mes[3];
    char anio[5];
} Fecha;

typedef struct Hora {
    char hora[3];
    char mins[3];
} Hora;

typedef struct Servicio {
    int clave;
    char nombre[21];
    float costo;
    char descripcion[51];
} Servicio;

typedef struct Cita {
    int claveCita;
    int claveServicio;
    char nomSolicitante[21];
    char nomPaciente[21];
    Conclusion conclusion;
    Fecha fecha;
    Hora hora;
} Cita;

void menu(int *opcion); //Menú principal
int leerString(char string[], int numChars); //Ingresa un string
int stringVacio(char string[]); //Revisa si el string está vacío o solo hay espacios
int numerico(char string[], int maxPuntos); //Revisa si el string es solo numérico y tiene menos de 'maxPuntos' puntos
int servRegistrado(); //Revisa si hay por lo menos un servicio registrado
int citaRegistrada(); //Revisa si hay por lo menos una cita registrada
int ingresaClave(int *clave, int file, Servicio *servicio, Cita *cita, int tipo); //Para registrar una clave para Servicio (tipo = 0) o Cita (tipo = 1)
int ingresaNombreDesc(char atributo[], int tipo); //Para registrar un nombre (tipo = 0) o descripción (tipo = 1)
int ingresaCosto(float *costo); //Para registrar el costo de un Servicio
void regServicio(); //Registro de servicio
int claveServParaCita(int *claveServicio); //Registra a una cita la clave del servicio a realizar
int ingreseFecha(Cita *cita, int file); //Registrar la fecha de una cita
int ingreseHora(Cita *cita, int file); //Registrar la hora de una cita
void regCita(); //Registro de cita
int buscaServicio(int file, Servicio *servicio); //Busca servicio
int buscaCita(int file, Cita *cita); //Busca cita
void servicioEspecifico(); //Consulta específica de servicios
void servicioGeneral(); //Consulta general de servicios
void menuConsultasServ(int *opcion); //Menú de consultas de servicios
void consultasServicios(int servicios); //Sección de consultas de servicios
void citaEspecifica(); //Consulta específica de citas
void citaGeneral(); //Consulta general de citas
void citasARealizar(); //Consulta de citas por realizar
void menuConsultasCitas(int *opcion); //Menú de consultas de citas
void consultasCitas(int citas); //Sección de consultas de citas
void menuConsultas(int *opcion); //Menú de consultas
void consultas(); //Sección de consultas
void eliminarServicio(); //Sección para eliminar un servicio
void eliminarCita(); //Elimina una cita
void concluirCita(); //Concluye una cita
void menuReprogramacion(int *opcion, Cita cita); //Menú de selección de reprogramación
void reprogramarCita(); //Reprograma una cita
void menuMods(int *opcion); //Menú de modificaciones
void modsCita(); //Sección de modificaciones a una cita

int main() {
    int opcion;

    menu(&opcion);
    while (opcion != 6) {
        switch (opcion) {
            case 1:
                regServicio();
                break;
            case 2:
                regCita();
                break;
            case 3:
                consultas();
                break;
            case 4:
                eliminarServicio();
                break;
            case 5:
                modsCita();
                break;
            default:
                printf("Esa opción no existe\n");
                break;
        }
        menu(&opcion);
    }
    return 0;
}

//Menú principal
void menu(int *opcion) {
    printf("1. Registrar servicio\n");
    printf("2. Registrar cita\n");
    printf("3. Consultas\n");
    printf("4. Eliminar servicio\n");
    printf("5. Modificar cita\n");
    printf("6. Salir\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Ingresa un string
int leerString(char string[], int numChars) {
    strcpy(string, "");
    switch (numChars) {
        case 5:
            scanf("%5[^\n]", string);
            break;
        case 8:
            scanf("%8[^\n]", string);
            break;
        case 9:
            scanf("%9[^\n]", string);
            break;
        case 10:
            scanf("%10[^\n]", string);
            break;
        case 20:
            scanf("%20[^\n]", string);
            break;
        case 50:
            scanf("%50[^\n]", string);
            break;
        default:
            printf("Error\n");
            break;
    }
    printf("\n");
    //Regresa error si se teclearon más de 'numChars' caracteres
    if (getchar() != '\n') {
        while (getchar() != '\n');
        return 1;
    }
    return 0;
}

//Revisa si el string está vacío o solo hay espacios
int stringVacio(char string[]) {
    int i;
    for (i = 0; i < strlen(string); i++) {
        if (string[i] != ' ') return 0;
    }
    return 2;
}

//Revisa si el string es solo numérico y tiene menos de 'maxPuntos' puntos
int numerico(char string[], int maxPuntos) {
    int i, puntos = 0;
    for (i = 0; i < strlen(string); i++) {
        if (!isdigit(string[i])) {
            if (string[i] != '.') return 3;
            else {
                if (puntos == maxPuntos) return 3;
                puntos++;
            }
        }
    }
    return 0;
}

//Revisa si hay por lo menos un servicio registrado
int servRegistrado() {
    int file, x;
    Servicio servicio;
    file = open("servicios.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) return 2;
    else {
        do {
            x = read(file, &servicio, sizeof(Servicio));
        } while (x != 0 && servicio.clave <= 0);
        close(file);
        if (x == 0) return 1;
        else return 0;
    }
}

//Revisa si hay por lo menos una cita registrada
int citaRegistrada() {
    int file, x;
    Cita cita;
    file = open("citas.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) return 2;
    else {
        do {
            x = read(file, &cita, sizeof(Cita));
        } while (x != 0 && cita.claveCita <= 0);
        close(file);
        if (x == 0) return 1;
        else return 0;
    }
}

//Para registrar una clave para Servicio (tipo = 0) o Cita (tipo = 1)
int ingresaClave(int *clave, int file, Servicio *servicio, Cita *cita, int tipo) {
    int error;
    char claveString[10];
    error = leerString(claveString, 9);
    if (error == 0) error = stringVacio(claveString);
    if (error == 0) error = numerico(claveString, 0);
    if (error == 0) {
        *clave = atoi(claveString);
        if ((*clave) == 0) error = 3;
    }
    if (error == 0) {
        if (tipo == 0) {
            lseek(file, ((*clave)-1)*sizeof(Servicio), SEEK_SET);
            read(file, servicio, sizeof(Servicio));
            if ((*clave) == servicio->clave) error = 4;
        } else {
            lseek(file, ((*clave)-1)*sizeof(Cita), SEEK_SET);
            read(file, cita, sizeof(Cita));
            if ((*clave) == cita->claveCita) error = 4;
        }
    }
    while (error != 0) {
        switch (error) {
            case 1:
                printf("La clave debe ser de máximo 9 dígitos");
                break;
            case 2:
                return 1;
            case 3:
                printf("La clave debe ser un número entero, positivo y no 0");
                break;
            case 4:
                printf("Esa clave ya está registrada");
                break;
        }
        printf(", ingrese otra:\n");
        error = leerString(claveString, 9);
        if (error == 0) error = stringVacio(claveString);
        if (error == 0) error = numerico(claveString, 0);
        if (error == 0) {
            *clave = atoi(claveString);
            if ((*clave) == 0) error = 3;
        }
        if (error == 0) {
            if (tipo == 0) {
                lseek(file, ((*clave)-1)*sizeof(Servicio), SEEK_SET);
                read(file, servicio, sizeof(Servicio));
                if ((*clave) == servicio->clave) error = 4;
            } else {
                lseek(file, ((*clave)-1)*sizeof(Cita), SEEK_SET);
                read(file, cita, sizeof(Cita));
                if ((*clave) == cita->claveCita) error = 4;
            }
        }
    }
    
    if (tipo == 0) {
        servicio->clave = *clave;
    } else {
        cita->claveCita = *clave;
    }
    return 0;
}

//Para registrar un nombre (tipo = 0) o descripción (tipo = 1)
int ingresaNombreDesc(char atributo[], int tipo) {
    int error;
    if (tipo == 0) error = leerString(atributo, 20);
    else error = leerString(atributo, 50);
    if (error == 0) error = stringVacio(atributo);
    while (error != 0) {
        switch (error) {
            case 1:
                if (tipo == 0) printf("El nombre debe ser de máximo 20 chars:\n");
                else printf("La descripción debe ser de máximo 50 chars:\n");
                break;
            case 2:
                return 1;
        }
        if (tipo == 0) error = leerString(atributo, 20);
        else error = leerString(atributo, 50);
        if (error == 0) error = stringVacio(atributo);
    }
    return 0;
}

//Para registrar el costo de un Servicio
int ingresaCosto(float *costo) {
    int error;
    char temp[9];
    error = leerString(temp, 8);
    if (error == 0) error = stringVacio(temp);
    if (error == 0) error = numerico(temp, 1);
    while (error != 0) {
        switch (error) {
            case 1:
                printf("El costo debe ser de máximo 8 dígitos, o 7 y un punto decimal:\n");
                break;
            case 2:
                return 1;
            case 3:
                printf("El costo debe ser positivo y con máximo un solo punto decimal:\n");
                break;
        }
        error = leerString(temp, 8);
        if (error == 0) error = stringVacio(temp);
        if (error == 0) error = numerico(temp, 1);
    }
    *costo = atof(temp);
    return 0;
}

//Registro de servicio
void regServicio() {
    int file, clave;
    Servicio servicio;
    file = open("servicios.txt", O_RDWR|O_CREAT, 0644);
    if (file == -1) {
        printf("No se pudo abrir el registro de servicios\n");
    } else {
        printf("Registro de servicio - Deje campo en blanco para cancelar y regresar al menú\n");
        printf("Ingrese clave de servicio (debe ser numérica, única, positiva y no 0):\n");
        if (ingresaClave(&clave, file, &servicio, NULL, 0) == 1) printf("Registro cancelado\n");
        else {
            printf("Ingrese nombre del servicio:\n");
            if (ingresaNombreDesc(servicio.nombre, 0) == 1) printf("Registro cancelado\n");
            else {
                printf("Ingrese costo del servicio:\n");
                if (ingresaCosto(&(servicio.costo)) == 1) printf("Registro cancelado\n");
                else {
                    printf("Ingrese descripción del servicio:\n");
                    if (ingresaNombreDesc(servicio.descripcion, 1) == 1) printf("Registro cancelado\n");
                    else {
                        lseek(file, (clave-1)*sizeof(Servicio), SEEK_SET);
                        write(file, &servicio, sizeof(Servicio));
                        printf("Registro de servicio exitoso\n");
                    }
                }
            }
        }
        close(file);
    }
}

//Registra a una cita la clave del servicio a realizar
int claveServParaCita(int *claveServicio) {
    int error, fileServicio;
    char claveString[10];
    Servicio servicio;

    fileServicio = open("servicios.txt", O_RDONLY|O_CREAT, 0644);
    if (fileServicio == -1) {
        printf("No se pudo abrir el registro de servicios\n");
        return 1;
    }
    error = leerString(claveString, 9);
    if (error == 0) error = stringVacio(claveString);
    if (error == 0) error = numerico(claveString, 0);
    if (error == 0) {
        *claveServicio = atoi(claveString);
        if ((*claveServicio) == 0) error = 3;
    }
    if (error == 0) {
        lseek(fileServicio, ((*claveServicio)-1)*sizeof(Servicio), SEEK_SET);
        read(fileServicio, &servicio, sizeof(Servicio));
        if ((*claveServicio) != servicio.clave) error = 4;
    }
    while (error != 0) {
        switch (error) {
            case 2:
                close(fileServicio);
                return 1;
            default:
                printf("No se encontró un servicio con esa clave, ingrese otra clave:\n");
                break;
        }
        error = leerString(claveString, 9);
        if (error == 0) error = stringVacio(claveString);
        if (error == 0) error = numerico(claveString, 0);
        if (error == 0) {
            *claveServicio = atoi(claveString);
            if ((*claveServicio) == 0) error = 3;
        }
        if (error == 0) {
            lseek(fileServicio, ((*claveServicio)-1)*sizeof(Servicio), SEEK_SET);
            read(fileServicio, &servicio, sizeof(Servicio));
            if ((*claveServicio) != servicio.clave) error = 4;
        }
    }
    close(fileServicio);
    return 0;
}

//Registrar la fecha de una cita
int ingreseFecha(Cita *cita, int file) {
    char fecha[11];
    Cita aux;
    int error, mins1, mins2;
    error = leerString(fecha, 10);
    if (error == 0) error = stringVacio(fecha);
    if (error == 0) {
        if (isdigit(fecha[0]) && isdigit(fecha[1]) && fecha[2] == '-' && isdigit(fecha[3]) && isdigit(fecha[4]) && fecha[5] == '-' && isdigit(fecha[6]) && isdigit(fecha[7]) && isdigit(fecha[8]) && isdigit(fecha[9])) {
            cita->fecha.dia[0] = fecha[0];
            cita->fecha.dia[1] = fecha[1];
            cita->fecha.dia[2] = '\0';
            cita->fecha.mes[0] = fecha[3];
            cita->fecha.mes[1] = fecha[4];
            cita->fecha.mes[2] = '\0';
            cita->fecha.anio[0] = fecha[6];
            cita->fecha.anio[1] = fecha[7];
            cita->fecha.anio[2] = fecha[8];
            cita->fecha.anio[3] = fecha[9];
            cita->fecha.anio[4] = '\0';
        } else error = 1;
    }
    if (error == 0 && strlen(cita->hora.hora) != 0) {
        mins1 = atoi(cita->hora.hora)*60 + atoi(cita->hora.mins); //Convierte hh:mm en minutos
        lseek(file, 0, SEEK_SET);
        error = read(file, &aux, sizeof(Cita));
        while (error != 0) {
            if (aux.claveCita > 0 && aux.claveCita != cita->claveCita && aux.conclusion == POR_REALIZARSE) {
                if (strcmp(cita->fecha.anio, aux.fecha.anio) == 0 && strcmp(cita->fecha.mes, aux.fecha.mes) == 0 && strcmp(cita->fecha.dia, aux.fecha.dia) == 0) {
                    mins2 = atoi(aux.hora.hora)*60 + atoi(aux.hora.mins); //Convierte hh:mm en minutos
                    if (mins1 == mins2 || (mins2 < mins1 && (mins2+59) >= mins1) || (mins1 < mins2 && (mins1+59) >= mins2)) break;
                }
            }
            error = read(file, &aux, sizeof(Cita));
        }
        if (error != 0) error = 3;
    }
    
    while (error != 0) {
        switch (error) {
            case 1:
                printf("Debe seguir el formato dd-mm-aaaa:\n");
                break;
            case 2:
                return 1;
            case 3:
                printf("La fecha ingresada con la hora existente se translapa con otra cita, ingrese otra fecha:\n");
                break;
        }
        error = leerString(fecha, 10);
        if (error == 0) error = stringVacio(fecha);
        if (error == 0) {
            if (isdigit(fecha[0]) && isdigit(fecha[1]) && fecha[2] == '-' && isdigit(fecha[3]) && isdigit(fecha[4]) && fecha[5] == '-' && isdigit(fecha[6]) && isdigit(fecha[7]) && isdigit(fecha[8]) && isdigit(fecha[9])) {
                cita->fecha.dia[0] = fecha[0];
                cita->fecha.dia[1] = fecha[1];
                cita->fecha.dia[2] = '\0';
                cita->fecha.mes[0] = fecha[3];
                cita->fecha.mes[1] = fecha[4];
                cita->fecha.mes[2] = '\0';
                cita->fecha.anio[0] = fecha[6];
                cita->fecha.anio[1] = fecha[7];
                cita->fecha.anio[2] = fecha[8];
                cita->fecha.anio[3] = fecha[9];
                cita->fecha.anio[4] = '\0';
            } else error = 1;
        }
        if (error == 0 && strlen(cita->hora.hora) != 0) {
            mins1 = atoi(cita->hora.hora)*60 + atoi(cita->hora.mins); //Convierte hh:mm en minutos
            lseek(file, 0, SEEK_SET);
            error = read(file, &aux, sizeof(Cita));
            while (error != 0) {
                if (aux.claveCita > 0 && aux.claveCita != cita->claveCita && aux.conclusion == POR_REALIZARSE) {
                    if (strcmp(cita->fecha.anio, aux.fecha.anio) == 0 && strcmp(cita->fecha.mes, aux.fecha.mes) == 0 && strcmp(cita->fecha.dia, aux.fecha.dia) == 0) {
                        mins2 = atoi(aux.hora.hora)*60 + atoi(aux.hora.mins); //Convierte hh:mm en minutos
                        if (mins1 == mins2 || (mins2 < mins1 && (mins2+59) >= mins1) || (mins1 < mins2 && (mins1+59) >= mins2)) break;
                    }
                }
                error = read(file, &aux, sizeof(Cita));
            }
            if (error != 0) error = 3;
        }
    } 
    return 0;
}

//Registrar la hora de una cita
int ingreseHora(Cita *cita, int file) {
    char hora[6];
    int error, mins1, mins2;
    Cita aux;
    error = leerString(hora, 5);
    if (error == 0) error = stringVacio(hora);
    if (error == 0) {
        if (isdigit(hora[0]) && isdigit(hora[1]) && hora[2] == ':' && isdigit(hora[3]) && isdigit(hora[4])) {
            cita->hora.hora[0] = hora[0];
            cita->hora.hora[1] = hora[1];
            cita->hora.hora[2] = '\0';
            cita->hora.mins[0] = hora[3];
            cita->hora.mins[1] = hora[4];
            cita->hora.mins[2] = '\0';
        } else error = 1;
    }
    if (error == 0) {
        mins1 = atoi(cita->hora.hora);
        mins2 = atoi(cita->hora.mins);
        if (mins1 > 23 || mins2 > 59) error = 3;
        else {
            mins1 = mins1*60 + mins2; //Convierte hh:mm en minutos
            if (mins1 < 480 || mins1 > 1200) error = 4; //480 mins = 8 horas, 1200 mins = 20 horas
        }
    }
    if (error == 0) {
        lseek(file, 0, SEEK_SET);
        error = read(file, &aux, sizeof(Cita));
        while (error != 0) {
            if (aux.claveCita > 0 && aux.claveCita != cita->claveCita && aux.conclusion == POR_REALIZARSE) {
                if (strcmp(cita->fecha.anio, aux.fecha.anio) == 0 && strcmp(cita->fecha.mes, aux.fecha.mes) == 0 && strcmp(cita->fecha.dia, aux.fecha.dia) == 0) {
                    mins2 = atoi(aux.hora.hora)*60 + atoi(aux.hora.mins); //Convierte hh:mm en minutos
                    if (mins1 == mins2 || (mins2 < mins1 && (mins2+59) >= mins1) || (mins1 < mins2 && (mins1+59) >= mins2)) break;
                }
            }
            error = read(file, &aux, sizeof(Cita));
        }
        if (error != 0) error = 5;
    }
    while (error != 0) {
        switch (error) {
            case 1:
                printf("Debe seguir el formato hh:mm\n");
                break;
            case 2:
                return 1;
            case 3:
                printf("Esa hora no existe, ingrese otra:\n");
                break;
            case 4:
                printf("Esa hora quedaría fuera de las horas de atención, ingrese otra:\n");
                break;
            case 5:
                printf("Esa hora se traslapa con otra cita, ingrese otra hora:\n");
                break;
        }
        error = leerString(hora, 5);
        if (error == 0) error = stringVacio(hora);
        if (error == 0) {
            if (isdigit(hora[0]) && isdigit(hora[1]) && hora[2] == ':' && isdigit(hora[3]) && isdigit(hora[4])) {
                cita->hora.hora[0] = hora[0];
                cita->hora.hora[1] = hora[1];
                cita->hora.hora[2] = '\0';
                cita->hora.mins[0] = hora[3];
                cita->hora.mins[1] = hora[4];
                cita->hora.mins[2] = '\0';
            } else error = 1;
        }
        if (error == 0) {
            mins1 = atoi(cita->hora.hora);
            mins2 = atoi(cita->hora.mins);
            if (mins1 > 23 || mins2 > 59) error = 3;
            else {
                mins1 = mins1*60 + mins2; //Convierte hh:mm en minutos
                if (mins1 < 480 || mins1 > 1200) error = 4; //480 mins = 8 horas, 1200 mins = 20 horas
            }
        }
        if (error == 0) {
            lseek(file, 0, SEEK_SET);
            error = read(file, &aux, sizeof(Cita));
            while (error != 0) {
                if (aux.claveCita > 0 && aux.claveCita != cita->claveCita && aux.conclusion == POR_REALIZARSE) {
                    if (strcmp(cita->fecha.anio, aux.fecha.anio) == 0 && strcmp(cita->fecha.mes, aux.fecha.mes) == 0 && strcmp(cita->fecha.dia, aux.fecha.dia) == 0) {
                        mins2 = atoi(aux.hora.hora)*60 + atoi(aux.hora.mins); //Convierte hh:mm en minutos
                        if (mins1 == mins2 || (mins2 < mins1 && (mins2+59) >= mins1) || (mins1 < mins2 && (mins1+59) >= mins2)) break;
                    }
                }
                error = read(file, &aux, sizeof(Cita));
            }
            if (error != 0) error = 5;
        }
    }
    return 0;
}

//Registro de cita
void regCita() {
    int fileCita, clave, error;
    Cita cita;

    error = servRegistrado();
    if (error == 1) {
        printf("De momento, no se ofrece ningún servicio\n");
        return;
    } else if (error == 2) {
        printf("No se pudo abrir el registro de servicios\n");
        return;
    }

    fileCita = open("citas.txt", O_RDWR|O_CREAT, 0644);
    if (fileCita == -1) {
        printf("No se pudo abrir el registro de citas\n");
    } else {
        printf("Registro de cita - Deje campo en blanco para cancelar y regresar al menú\n");
        printf("Ingrese clave de cita (debe ser numérica, única, positiva y no 0):\n");
        if (ingresaClave(&clave, fileCita, NULL, &cita, 1) == 1) printf("Registro cancelado\n");
        else {
            printf("Ingrese clave del servicio a realizar:\n");
            if (claveServParaCita(&(cita.claveServicio)) == 1) printf("Registro cancelado\n");
            else {
                printf("Ingrese nombre del solicitante:\n");
                if (ingresaNombreDesc(cita.nomSolicitante, 0) == 1) printf("Registro cancelado\n");
                else {
                    printf("Ingrese nombre del paciente:\n");
                    if (ingresaNombreDesc(cita.nomPaciente, 0) == 1) printf("Registro cancelado\n");
                    else {
                        cita.hora.hora[0] = '\0';
                        printf("Ingrese fecha para la cita en formato dd-mm-aaaa:\n");
                        if (ingreseFecha(&cita, fileCita) == 1) printf("Registro cancelado\n");
                        else {
                            printf("Horario de atención: 08:00 a 21:00\n");
                            printf("Cada cita dura 1 hora exacta\n");
                            printf("Ingrese hora para la cita en formato de 24 horas hh:mm\n");
                            if (ingreseHora(&cita, fileCita) == 1) printf("Registro cancelado\n");
                            else {
                                cita.conclusion = POR_REALIZARSE;
                                lseek(fileCita, (clave-1)*sizeof(Cita), SEEK_SET);
                                write(fileCita, &cita, sizeof(Cita));
                                printf("Registro de cita exitoso\n");
                            }
                        }
                    }
                }
            }
        }
        close(fileCita);
    }
}

//Busca servicio
int buscaServicio(int file, Servicio *servicio) {
    char temp[10];
    int clave;
    if (leerString(temp, 9) != 0 || stringVacio(temp) != 0 || numerico(temp, 0) != 0) {
        return 1;
    } else {
        clave = atoi(temp);
        if (clave == 0) return 1;
        lseek(file, (clave-1)*sizeof(Servicio), SEEK_SET);
        read(file, servicio, sizeof(Servicio));
        if (servicio->clave == clave) return 0;
        else return 1;
    }
}

//Busca cita
int buscaCita(int file, Cita *cita) {
    char temp[10];
    int clave;
    if (leerString(temp, 9) != 0 || stringVacio(temp) != 0 || numerico(temp, 0) != 0) {
        return 1;
    } else {
        clave = atoi(temp);
        if (clave == 0) return 1;
        lseek(file, (clave-1)*sizeof(Cita), SEEK_SET);
        read(file, cita, sizeof(Cita));
        if (cita->claveCita == clave) return 0;
        else return 1;
    }
}

//Consulta específica de servicios
void servicioEspecifico() {
    int file, clave;
    char temp[10];
    Servicio servicio;
    file = open("servicios.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) printf("No pudo abrirse el registro de servicios\n");
    else {
        printf("Ingrese la clave del servicio a consultar:\n");
        if (buscaServicio(file, &servicio) == 0) {
            printf("-----Servicio-----\n");
            printf("%-9s   %-20s   %-9s   %s\n", "Clave", "Nombre", "Costo", "Descripción");
            printf("%-9d   %-20s   $%-8.2f   %s\n", servicio.clave, servicio.nombre, servicio.costo, servicio.descripcion);
            printf("-----Servicio-----\n");
        } else printf("No se encontró un servicio con esa clave\n");
        close(file);
    }
}

//Consulta general de servicios
void servicioGeneral() {
    int file;
    Servicio servicio;
    file = open("servicios.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) printf("No pudo abrirse el registro de servicios\n");
    else {
        printf("-----Servicios-----\n");
        printf("%-9s   %-20s   %-9s   %s\n", "Clave", "Nombre", "Costo", "Descripción");
        while (read(file, &servicio, sizeof(Servicio)) != 0) {
            if (servicio.clave > 0)
                printf("%-9d   %-20s   $%-8.2f   %s\n", servicio.clave, servicio.nombre, servicio.costo, servicio.descripcion);
        }
        printf("-----Servicios-----\n");
        close(file);
    }
}

//Menú de consultas de servicios
void menuConsultasServ(int *opcion) {
    printf("1. Consulta específica\n");
    printf("2. Consulta general\n");
    printf("3. Regresar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Sección de consultas de servicios
void consultasServicios(int servicios) {
    int opcion;
    
    if (servicios != 0) {
        printf("No hay servicios registrados\n");
        return;
    }

    menuConsultasServ(&opcion);
    while (opcion != 3) {
        switch (opcion) {
            case 1:
                servicioEspecifico();
                break;
            case 2:
                servicioGeneral();
                break;
            default:
                printf("Esa opción no existe\n");
                break;
        }
        menuConsultasServ(&opcion);
    }
}

//Consulta específica de citas
void citaEspecifica() {
    int file, clave;
    char temp[10];
    Cita cita;
    file = open("citas.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) printf("No pudo abrirse el registro de citas\n");
    else {
        printf("Ingrese la clave de la cita a consultar:\n");
        if (buscaCita(file, &cita) == 0) {
            printf("-----Cita-----\n");
            printf("%-13s   %-17s   %-20s   %-20s   %-15s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Conclusión", "Fecha", "Hora");
            printf("%-13d   %-17d   %-20s   %-20s   ", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
            if (cita.conclusion == POR_REALIZARSE) printf("%-14s", "Por realizarse");
            else printf("%-14s", "Concluida");
            printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
            printf("-----Cita-----\n");
        } else printf("No se encontró una cita con esa clave\n");
        close(file);
    }
}

//Consulta general de citas
void citaGeneral() {
    int file;
    Cita cita;
    file = open("citas.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) printf("No pudo abrirse el registro de citas\n");
    else {
        printf("-----Citas-----\n");
        printf("%-13s   %-17s   %-20s   %-20s   %-15s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Conclusión", "Fecha", "Hora");
        while (read(file, &cita, sizeof(Cita)) != 0) {
            if (cita.claveCita > 0) {
                printf("%-13d   %-17d   %-20s   %-20s   ", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
                if (cita.conclusion == POR_REALIZARSE) printf("%-14s", "Por realizarse");
                else printf("%-14s", "Concluida");
                printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
            }
        }
        printf("-----Citas-----\n");
        close(file);
    }
}

//Consulta de citas por realizar
void citasARealizar() {
    int file, x;
    Cita cita;
    file = open("citas.txt", O_RDONLY|O_CREAT, 0644);
    if (file == -1) printf("No pudo abrirse el registro de citas\n");
    else {

        do {
            x = read(file, &cita, sizeof(Cita));
            if (cita.claveCita > 0 && cita.conclusion == POR_REALIZARSE) break;
        } while (x != 0);
        if (x == 0) {
            printf("No hay citas no concluidas\n");
            return;
        }

        printf("-----Citas no concluidas-----\n");
        printf("%-13s   %-17s   %-20s   %-20s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Fecha", "Hora");
        do {
            if (cita.claveCita > 0 && cita.conclusion == POR_REALIZARSE) {
                printf("%-13d   %-17d   %-20s   %-20s", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
                printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
            }
        } while (read(file, &cita, sizeof(Cita)) != 0);
        printf("-----Citas no concluidas-----\n");
        close(file);
    }
}

//Menú de consultas de citas
void menuConsultasCitas(int *opcion) {
    printf("1. Consulta específica de cita\n");
    printf("2. Consulta general de citas\n");
    printf("3. Consulta de citas por realizar\n");
    printf("4. Regresar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Sección de consultas de citas
void consultasCitas(int citas) {
    int opcion;

    if (citas != 0) {
        printf("No hay citas registradas\n");
        return;
    }

    menuConsultasCitas(&opcion);
    while (opcion != 4) {
        switch (opcion) {
            case 1:
                citaEspecifica();
                break;
            case 2:
                citaGeneral();
                break;
            case 3:
                citasARealizar();
                break;
            default:
                printf("Esa opción no existe\n");
                break;
        }
        menuConsultasCitas(&opcion);
    }
}

//Menú de consultas
void menuConsultas(int *opcion) {
    printf("1. Servicios\n");
    printf("2. Citas\n");
    printf("3. Regresar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Sección de consultas
void consultas() {
    int opcion, servicios, citas;

    servicios = servRegistrado();
    citas = citaRegistrada();

    if (servicios != 0 && citas != 0) {
        printf("No hay ningun servicio ni cita registrados\n");
        return;
    }

    menuConsultas(&opcion);
    while (opcion != 3) {
        switch (opcion) {
            case 1:
                consultasServicios(servicios);
                break;
            case 2:
                consultasCitas(citas);
                break;
            default:
                printf("Esa opción no existe\n");
                break;
        }
        menuConsultas(&opcion);
    }
}

//Sección para eliminar un servicio
void eliminarServicio() {
    int fileServicio, fileCita, fileNuevo, error, opcion, clave;
    Servicio servicio;
    Cita cita;

    if (servRegistrado() != 0) {
        printf("No hay servicios registrados\n");
        return;
    }

    fileServicio = open("servicios.txt", O_RDONLY|O_CREAT, 0644);
    fileCita = open("citas.txt", O_RDONLY|O_CREAT, 0644);

    if (fileServicio == -1) printf("No se pudo abrir el registro de servicios\n");
    else if (fileCita == -1) printf("No se pudo abrir el registro de citas\n");
    else {
        printf("Ingrese la clave del servicio a eliminar:\n");
        if (buscaServicio(fileServicio, &servicio) == 0) {
            do {
                error = read(fileCita, &cita, sizeof(Cita));
                if (cita.claveCita > 0 && cita.claveServicio == servicio.clave && cita.conclusion == POR_REALIZARSE) break;
            } while (error != 0);
            if (error == 0) {
                printf("%-9s   %-20s   %-9s   %s\n", "Clave", "Nombre", "Costo", "Descripción");
                printf("%-9d   %-20s   $%-8.2f   %s\n", servicio.clave, servicio.nombre, servicio.costo, servicio.descripcion);
                printf("¿Seguro que desea eliminar este servicio?\n1. Sí\n2. No\n");
                scanf("%d", &opcion);
                while (getchar() != '\n');
                while (opcion != 1 && opcion != 2) {
                    printf("\n1. Sí\n2. No\n");
                    scanf("%d", &opcion);
                    while (getchar() != '\n');
                }
                printf("\n");
                if (opcion == 1) {
                    fileNuevo = open("aux.txt", O_WRONLY|O_CREAT, 0644);
                    if (fileNuevo == -1) {
                        printf("No se pudo eliminar el servicio\n");
                    } else {
                        close(fileCita);
                        lseek(fileServicio, 0, SEEK_SET);
                        clave = servicio.clave;
                        while (read(fileServicio, &servicio, sizeof(Servicio)) != 0) {
                            if (servicio.clave > 0 && servicio.clave != clave) {
                                lseek(fileNuevo, ((servicio.clave)-1)*sizeof(Servicio), SEEK_SET);
                                write(fileNuevo, &servicio, sizeof(Servicio));
                            }
                        }
                        close(fileServicio);
                        unlink("servicios.txt");
                        close(fileNuevo);
                        rename("aux.txt", "servicios.txt");
                        printf("Servicio eliminado\n");
                        return;
                    }
                } else {
                    printf("No se eliminó el servicio\n");
                }
            } else {
                printf("Hay citas por realizar de este servicio\n");
            } 
        } else {
            printf("No se encontró un servicio con esa clave\n");
        }
        close(fileServicio);
        close(fileCita);
    }
}

//Elimina una cita
void eliminarCita() {
    int fileCita, opcion, clave;
    Cita cita;

    fileCita = open("citas.txt", O_RDWR|O_CREAT, 0644);
    if (fileCita == -1) printf("No se pudo abrir el registro de citas\n");
    else {
        printf("Ingrese la clave de la cita a eliminar:\n");
        if (buscaCita(fileCita, &cita) == 0) {
            if (cita.conclusion == CONCLUIDA) {
                printf("%-13s   %-17s   %-20s   %-20s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Fecha", "Hora");
                printf("%-13d   %-17d   %-20s   %-20s", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
                printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
                printf("¿Seguro que desea eliminar esta cita?\n1. Sí\n2. No\n");
                scanf("%d", &opcion);
                while (getchar() != '\n');
                while (opcion != 1 && opcion != 2) {
                    printf("\n1. Sí\n2. No\n");
                    scanf("%d", &opcion);
                    while (getchar() != '\n');
                }
                printf("\n");
                if (opcion == 1) {
                    clave = cita.claveCita;
                    cita.claveCita = 0;
                    lseek(fileCita, (clave-1)*sizeof(Cita), SEEK_SET);
                    write(fileCita, &cita, sizeof(Cita));
                    printf("Cita eliminada\n");
                } else {
                    printf("No se eliminó la cita\n");
                }
            } else { 
                printf("Esta cita no está concluida\n");
            } 
        } else {
            printf("No se encontró una cita con esa clave\n");
        }
        close(fileCita);
    }
}

//Concluye una cita
void concluirCita() {
    int fileCita, opcion;
    Cita cita;

    fileCita = open("citas.txt", O_RDWR|O_CREAT, 0644);
    if (fileCita == -1) printf("No se pudo abrir el registro de citas\n");
    else {
        printf("Ingrese la clave de la cita a concluir:\n");
        if (buscaCita(fileCita, &cita) == 0) {
            if (cita.conclusion == POR_REALIZARSE) {
                printf("%-13s   %-17s   %-20s   %-20s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Fecha", "Hora");
                printf("%-13d   %-17d   %-20s   %-20s", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
                printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
                printf("¿Seguro que desea concluir esta cita?\n1. Sí\n2. No\n");
                scanf("%d", &opcion);
                while (getchar() != '\n');
                while (opcion != 1 && opcion != 2) {
                    printf("\n1. Sí\n2. No\n");
                    scanf("%d", &opcion);
                    while (getchar() != '\n');
                }
                printf("\n");
                if (opcion == 1) {
                    cita.conclusion = CONCLUIDA;
                    lseek(fileCita, ((cita.claveCita)-1)*sizeof(Cita), SEEK_SET);
                    write(fileCita, &cita, sizeof(Cita));
                    printf("Cita concluida\n");
                } else {
                    printf("No se concluyó la cita\n");
                }
            } else { 
                printf("Esta cita ya está concluida\n");
            } 
        } else {
            printf("No se encontró una cita con esa clave\n");
        }
        close(fileCita);
    }
}

//Menú de selección de reprogramación
void menuReprogramacion(int *opcion, Cita cita) {
    printf("%-13s   %-17s   %-20s   %-20s   %-10s   %s\n", "Clave de Cita", "Clave de Servicio", "Solicitante", "Paciente", "Fecha", "Hora");
    printf("%-13d   %-17d   %-20s   %-20s", cita.claveCita, cita.claveServicio, cita.nomSolicitante, cita.nomPaciente);
    printf("   %s-%s-%s   %s:%s\n", cita.fecha.dia, cita.fecha.mes, cita.fecha.anio, cita.hora.hora, cita.hora.mins);
    printf("1. Modificar fecha\n2. Modificar hora\n3. Regresar\n");
    scanf("%d", opcion);
    printf("\n");
    while (getchar() != '\n');
}


//Reprograma una cita
void reprogramarCita() {
    int fileCita, opcion;
    Cita cita;

    fileCita = open("citas.txt", O_RDWR|O_CREAT, 0644);
    if (fileCita == -1) printf("No se pudo abrir el registro de citas\n");
    else {
        printf("Ingrese la clave de la cita a reprogramar:\n");
        if (buscaCita(fileCita, &cita) == 0) {
            if (cita.conclusion == POR_REALIZARSE) {
                menuReprogramacion(&opcion, cita);
                while (opcion != 3) {
                    switch (opcion) {
                        case 1:
                            printf("Ingrese nueva fecha en formato dd-mm-aaaa, o deje en blanco para cancelar:\n");
                            if (ingreseFecha(&cita, fileCita) == 0) {
                                lseek(fileCita, ((cita.claveCita)-1)*sizeof(Cita), SEEK_SET);
                                write(fileCita, &cita, sizeof(Cita));
                                printf("Fecha modificada\n");
                            } else {
                                lseek(fileCita, ((cita.claveCita)-1)*sizeof(Cita), SEEK_SET);
                                read(fileCita, &cita, sizeof(Cita));
                                printf("Fecha no modificada\n");
                            }
                            break;
                        case 2:
                            printf("Ingrese nueva hora en formato hh:mm, o deje en blanco para cancelar:\n");
                            if (ingreseHora(&cita, fileCita) == 0) {
                                lseek(fileCita, ((cita.claveCita)-1)*sizeof(Cita), SEEK_SET);
                                write(fileCita, &cita, sizeof(Cita));
                                printf("Hora modificada\n");
                            } else {
                                lseek(fileCita, ((cita.claveCita)-1)*sizeof(Cita), SEEK_SET);
                                read(fileCita, &cita, sizeof(Cita));
                                printf("Hora no modificada\n");
                            }
                            break;
                        default:
                            printf("Esa opción no existe\n");
                            break;
                    }
                    menuReprogramacion(&opcion, cita);
                }
            } else { 
                printf("Esta cita ya está concluida\n");
            } 
        } else {
            printf("No se encontró una cita con esa clave\n");
        }
        close(fileCita);
    }
}

//Menú de modificaciones
void menuMods(int *opcion) {
    printf("1. Eliminar cita\n");
    printf("2. Concluir cita\n");
    printf("3. Reprogramar cita\n");
    printf("4. Regresar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Sección de modificaciones a una cita
void modsCita() {
    int opcion;
    
    if (citaRegistrada() != 0) {
        printf("No hay citas registradas\n");
        return;
    }

    menuMods(&opcion);
    while (opcion != 4) {
        switch (opcion) {
            case 1:
                eliminarCita();
                break;
            case 2:
                concluirCita();
                break;
            case 3:
                reprogramarCita();
                break;
            default:
                printf("Esa opción no existe\n");
                break;
        }
        menuMods(&opcion);
    }
}
