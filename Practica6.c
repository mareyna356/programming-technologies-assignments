#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define EDAD_MAXIMA 20

typedef struct Persona {
    int ID;
    char nombre[17];
    int edad;
    int nacimiento;
    int estatura;
    int peso;
    struct Persona *siguiente;
} Persona;

int leerString(char string[]); //Ingresa un string
int stringVacio(char string[]); //Revisa si el string está vacío o solo hay espacios
int numerico(char string[]); //Revisa si el string es solo numérico
int alfabetico(char string[]); //Revisa si el string es solo alfabético
void menu(int *opcion, int anioActual); //Menú principal
void buscaID(int ID, Persona **persona); //Busca una ID registrada
int ingresaID(Persona *inicial, int *personaID); //Ingresa una ID para registrar en una nueva persona
int ingresaNombre(char nombre[]); //Ingresa un nombre para registrar en una nueva persona
int ingresaEstOPeso(int *personaAtributo, int limite); /*Ingresa estatura (tipo = 1) o peso (tipo = 2)
                                                         para registrar en una nueva persona*/
void nacimiento(Persona **nuevo, int anioActual); //Proceso del nuevo nacimiento
void avanzaTiempo(Persona **vivos, Persona **borrados, int *anioActual); //Avanza el tiempo
void menuConsultas(int *opcion); //Menú de consultas
void consultaGeneral(Persona *lista); //Consulta general
void consultaID(Persona *lista); //Consulta por ID
void promedios(Persona *lista); //Consulta de promedios de edad, estatura y peso
void consultas(Persona *vivos, Persona *borrados); //Sección de consultas
void terminar(Persona *personas); //Libera toda la memoria de la lista

int main() {
    int opcion, anioActual = 2000;
    Persona *primero = NULL, *primeroBorrados = NULL;
    srand(time(0));
    
    menu(&opcion, anioActual);
    while (opcion != 4) {
        switch (opcion) {
            case 1:
                nacimiento(&primero, anioActual);
                break;
            case 2:
                avanzaTiempo(&primero, &primeroBorrados, &anioActual);
                break;
            case 3:
                consultas(primero, primeroBorrados);
                break;
            default:
                printf("Esa opción no está disponible\n");
                break;
        }
        menu(&opcion, anioActual);
    }
    terminar(primero);
    terminar(primeroBorrados);

    return 0;
}

//Ingresa un string
int leerString(char string[]) {
    strcpy(string, "");
    scanf("%16[^\n]", string);
    //Regresa error si se teclearon más de 16 caracteres
    if (getchar() != '\n') {
        while (getchar() != '\n');
        printf("\n");
        return 1;
    }
    printf("\n");
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

//Revisa si el string es solo numérico
int numerico(char string[]) {
    int i;
    for (i = 0; i < strlen(string); i++) {
        if (!isdigit(string[i])) {
            return 3;
        }
    }
    return 0;
}

//Revisa si el string es solo alfabético
int alfabetico(char string[]) {
    int i;
    for (i = 0; i < strlen(string); i++) {
        if (!isalpha(string[i]) && string[i] != ' ' && string[i] != '-') {
            return 3;
        }
    }
    return 0;
}

//Menú principal
void menu(int *opcion, int anioActual) {
    printf("Año actual: %d\n", anioActual);
    printf("[1] Nuevo nacimiento\n");
    printf("[2] Avanzar tiempo\n");
    printf("[3] Consultas\n");
    printf("[4] Terminar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Busca una ID registrada
void buscaID(int ID, Persona **persona) {
    while ((*persona) != NULL) {
        if ((*persona)->ID == ID) return;
        *persona = (*persona)->siguiente;
    }
}

//Ingresa una ID para registrar en una nueva persona
int ingresaID(Persona *inicial, int *personaID) {
    int error;
    char ID[17];
    Persona *aux = inicial;
    error = leerString(ID);
    if (error == 0 && strlen(ID) > 9) error = 1; //Límite de int es 2,147,483,647
    if (error == 0) error = stringVacio(ID);
    if (error == 0) error = numerico(ID);
    if (error == 0) {
        *personaID = atoi(ID);
        buscaID(*personaID, &inicial);
        if (inicial != NULL) error = 4;
    }
    while (error != 0) {
        switch (error) {
            case 1:
                printf("La ID debe ser de máximo 9 dígitos");
                break;
            case 2:
                return 1;
            case 3:
                printf("La ID debe ser un número entero y positivo");
                break;
            case 4:
                printf("Esa ID ya está registrada");
                break;
        }
        printf(", ingrese otra:\n");
        error = leerString(ID);
        if (error == 0 && strlen(ID) > 9) error = 1; //Límite de int es 2,147,483,647
        if (error == 0) error = stringVacio(ID);
        if (error == 0) error = numerico(ID);
        if (error == 0) {
            inicial = aux;
            *personaID = atoi(ID);
            buscaID(*personaID, &inicial);
            if (inicial != NULL) error = 4;
        }
    }
    return 0;
}

//Ingresa un nombre para registrar en una nueva persona
int ingresaNombre(char nombre[]) {
    int error;
    error = leerString(nombre);
    if (error == 0) error = stringVacio(nombre);
    if (error == 0) error = alfabetico(nombre);
    while (error != 0) {
        switch (error) {
            case 1:
                printf("El nombre debe ser de máximo 16 caracteres");
                break;
            case 2:
                return 1;
            case 3:
                printf("El nombre solo puede tener letras, espacios y guiones");
                break;
        }
        printf(", ingrese otro:\n");
        error = leerString(nombre);
        if (error == 0) error = stringVacio(nombre);
        if (error == 0) error = alfabetico(nombre);
    }
    return 0;
}

//Ingresa estatura (limite = 200) o peso (limite = 100) para registrar en una nueva persona
int ingresaEstOPeso(int *personaAtributo, int limite) {
    int error;
    char atributo[17];
    error = leerString(atributo);
    if (error == 0) error = stringVacio(atributo);
    if (error == 0) error = numerico(atributo);
    if (error == 0) {
        *personaAtributo = atoi(atributo);
        if ((*personaAtributo) < 1 || (*personaAtributo) > limite) error = 1;
    }
    while (error != 0) {
        switch (error) {
            case 1:
                if (limite == 200) printf("La estatura debe ser mínimo 1cm y máximo 200cm");
                else printf("El peso debe ser mínimo 1kg y máximo 100kg");
                break;
            case 2:
                return 1;
            case 3:
                printf("Debe ser un número entero y positivo");
                break;
        }
        if (limite == 200) printf(", ingrese otra:\n");
        else printf(", ingrese otro:\n");
        error = leerString(atributo);
        if (error == 0) error = stringVacio(atributo);
        if (error == 0) error = numerico(atributo);
        if (error == 0) {
            *personaAtributo = atoi(atributo);
            if ((*personaAtributo) < 1 || (*personaAtributo) > limite) error = 1;
        }
    }
    return 0;
}

//Proceso del nuevo nacimiento
void nacimiento(Persona **nuevo, int anioActual) {
    int error;
    Persona *aux = *nuevo;
    *nuevo = (Persona*)malloc(sizeof(Persona));
    if ((*nuevo) == NULL) {
        *nuevo = aux;
        printf("Memoria llena\n");
    } else {
        printf("Registrando nueva persona - Deje el campo en blanco para regresar al menú\n");
        printf("Ingrese número único de identificación de máximo 9 dígitos:\n");
        error = ingresaID(aux, &((*nuevo)->ID));
        if (error == 0) {
            printf("Ingrese nombre de máximo 16 caracteres:\n");
            error = ingresaNombre((*nuevo)->nombre);
            if (error == 0) {
                (*nuevo)->edad = 0;
                (*nuevo)->nacimiento = anioActual;
                printf("Ingrese estatura en cm:\n");
                error = ingresaEstOPeso(&((*nuevo)->estatura), 200);
                if (error == 0) {
                    printf("Ingrese peso en kg:\n");
                    error = ingresaEstOPeso(&((*nuevo)->peso), 100);
                }
            }
        }
        if (error == 1) {
            free(*nuevo);
            *nuevo = aux;
            printf("Registro cancelado\n");
        } else {
            (*nuevo)->siguiente = aux;
            printf("Registro exitoso\n");
        }
    }
}

//Avanza el tiempo
void avanzaTiempo(Persona **vivos, Persona **borrados, int *anioActual) {
    int temp, random;
    Persona *aux = NULL, *aux2 = NULL;
    if ((*vivos) == NULL) printf("No hay personas registradas\n");
    else {
        printf("¿Cuántos años quiere avanzar? 0 para regresar\n");
        scanf("%d", &temp);
        while (getchar() != '\n');
        printf("\n");
        while (temp < 0) {
            printf("Introduzca una cantidad no negativa o 0:\n");
            scanf("%d", &temp);
            while (getchar() != '\n');
            printf("\n");
        }
        if (temp == 0) printf("No se avanzó el tiempo\n");
        else {
            for (temp; temp > 0; temp--) {
                (*anioActual)++;
                aux = *vivos;
                while (aux != NULL) {
                    if ((aux->estatura) < 200) {
                        random = 1 + (rand() % 5); //Genera un número aleatorio entre 1 y 5;
                        aux->estatura += random;
                        if ((aux->estatura) > 200) aux->estatura = 200;
                    }
                    if ((aux->peso) < 100) {
                        random = 1 + (rand() % 10); //Genera un número aleatorio entre 1 y 10;
                        aux->peso += random;
                        if ((aux->peso) > 100) aux->peso = 100;
                    }
                    (aux->edad)++;
                    if ((aux->edad) > EDAD_MAXIMA) {
                        if (aux == *vivos) {
                            *vivos = (*vivos)->siguiente;
                            aux->siguiente = *borrados;
                            *borrados = aux;
                            aux = *vivos;
                        } else {
                            aux = aux->siguiente;
                            aux2->siguiente->siguiente = *borrados;
                            *borrados = aux2->siguiente;
                            aux2->siguiente = aux;
                        }
                    } else {
                        aux2 = aux;
                        aux = aux->siguiente;
                    }
                }
            }
        }
    }
}

//Menú de consultas
void menuConsultas(int *opcion) {
    printf("[1] Consulta general\n");
    printf("[2] Consulta por número de indentificación\n");
    printf("[3] Mostrar el promedio de edad, estatura y peso\n");
    printf("[4] Consulta general de personas eliminadas\n");
    printf("[5] Regresar\n");
    scanf("%d", opcion);
    while (getchar() != '\n');
    printf("\n");
}

//Consulta general
void consultaGeneral(Persona *lista) {
    if (lista == NULL) printf("No hay personas registradas en esta lista\n");
    else {
        printf("%-9s | %-16s | %-4s | %-18s | %-8s | %s\n", "ID", "Nombre", "Edad", "Año de nacimiento", "Estatura", "Peso");
        while (lista != NULL) {
            printf("%-9d | %-16s | %-4d | %-17d | %-3d cm   | %d kg\n", lista->ID, lista->nombre, lista->edad, lista->nacimiento, lista->estatura, lista->peso);
            lista = lista->siguiente;
        }
    }
    printf("\n");
}

//Consulta por ID
void consultaID(Persona *lista) {
    int ID;
    if (lista == NULL) printf("No hay personas registradas en la lista\n");
    else {
        printf("Ingrese ID de la persona a consultar:\n");
        scanf("%d", &ID);
        while (getchar() != '\n');
        printf("\n");
        buscaID(ID, &lista);
        if (lista == NULL) printf("No se encontró una persona con esa ID\n");
        else {
            printf("%-9s | %-16s | %-4s | %-18s | %-8s | %s\n", "ID", "Nombre", "Edad", "Año de nacimiento", "Estatura", "Peso");
            printf("%-9d | %-16s | %-4d | %-17d | %-3d cm   | %d kg\n\n", lista->ID, lista->nombre, lista->edad, lista->nacimiento, lista->estatura, lista->peso);
        }
    }
}

//Consulta de promedios de edad, estatura y peso
void promedios(Persona *lista) {
    int edad = 0, estatura = 0, peso = 0, cuenta = 0;
    if (lista == NULL) printf("No hay personas registradas en la lista\n");
    else {
        while (lista != NULL) {
            edad += lista->edad;
            estatura += lista->estatura;
            peso += lista->peso;
            cuenta++;
            lista = lista->siguiente;
        }
        printf("Promedio de %d personas:\n", cuenta);
        printf("Edad: %f años\nEstatura: %f cm\nPeso: %f kg\n\n", 1.0*edad/cuenta, 1.0*estatura/cuenta, 1.0*peso/cuenta);
    }
}

//Sección de consultas
void consultas(Persona *vivos, Persona *borrados) {
    int opcion;
    if (vivos == NULL && borrados == NULL) {
        printf("No hay personas registradas ni borradas\n");
        return;
    }
    menuConsultas(&opcion);
    while (opcion != 5) {
        switch (opcion) {
            case 1:
                consultaGeneral(vivos);
                break;
            case 2:
                consultaID(vivos);
                break;
            case 3:
                promedios(vivos);
                break;
            case 4:
                consultaGeneral(borrados);
                break;
            default:
                printf("Esa opción no está disponible\n");
                break;
        }
        menuConsultas(&opcion);
    }
}

//Libera toda la memoria de la lista
void terminar(Persona *personas) {
    Persona *aux = personas;
    while (aux != NULL) {
        personas = personas->siguiente;
        free(aux);
        aux = personas;
    }
}
