#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * LIFO (Last In, First Out)
 * Basado en la lista doblemente ligada del programa lista_ligada.c.
 *
 * Insercion: al final de la lista.
 * Extraccion: tambien desde el final de la lista.
 */

typedef unsigned char byte;

typedef struct comparte
{
    char nombre[50];
    char direccion[50];
    int edad;
    struct comparte *next;
    struct comparte *prev;
} Registro;

typedef struct l
{
    Registro *Inicial;
    Registro *Final;
    int len;
} Lista;

/* Crea un nodo de tipo Registro. */
Registro *crear(void)
{
    Registro *T;

    T = (Registro *)malloc(sizeof(Registro));

    if (T == NULL)
        return NULL;

    T->next = NULL;
    T->prev = NULL;

    return T;
}

/* Inicializa la lista con dos nodos centinela: Inicial y Final. */
int crear_lista(Lista *lista)
{
    Registro *N_inicial;
    Registro *N_final;

    N_inicial = crear();
    N_final = crear();

    if (N_inicial == NULL || N_final == NULL)
    {
        free(N_inicial);
        free(N_final);
        return 0;
    }

    lista->Inicial = N_inicial;
    lista->Final = N_final;
    lista->len = 0;

    lista->Inicial->next = N_final;
    lista->Inicial->prev = NULL;

    lista->Final->prev = N_inicial;
    lista->Final->next = NULL;

    return 1;
}

/*
 * Agrega un elemento al FINAL.
 * Esta operacion corresponde al PUSH de una pila LIFO.
 */
int agregar(Lista *lista, const char *nombre,
            const char *direccion, int edad)
{
    Registro *N;

    N = crear();

    if (N == NULL)
        return 0;

    strcpy(N->nombre, nombre);
    strcpy(N->direccion, direccion);
    N->edad = edad;

    N->prev = lista->Final->prev;
    N->next = lista->Final;

    lista->Final->prev->next = N;
    lista->Final->prev = N;

    lista->len++;

    return 1;
}

/*
 * Extrae el ULTIMO elemento que fue agregado.
 * LIFO: Last In, First Out.
 *
 * El registro extraido se copia en *dato.
 * Regresa 1 si se extrajo correctamente y 0 si la pila esta vacia.
 */
int extraer_lifo(Lista *lista, Registro *dato)
{
    Registro *ultimo;

    if (lista->len == 0)
        return 0;

    ultimo = lista->Final->prev;

    strcpy(dato->nombre, ultimo->nombre);
    strcpy(dato->direccion, ultimo->direccion);
    dato->edad = ultimo->edad;
    dato->next = NULL;
    dato->prev = NULL;

    lista->Final->prev = ultimo->prev;
    ultimo->prev->next = lista->Final;

    free(ultimo);
    lista->len--;

    return 1;
}

/* Imprime los elementos desde el primero hasta el ultimo. */
void print_list(Lista *lista)
{
    Registro *iter;
    int posicion = 1;

    if (lista->len == 0)
    {
        printf("La pila esta vacia.\n");
        return;
    }

    for (iter = lista->Inicial->next;
         iter != lista->Final;
         iter = iter->next)
    {
        printf("Elemento %d\n", posicion);
        printf("  Nombre:    %s\n", iter->nombre);
        printf("  Direccion: %s\n", iter->direccion);
        printf("  Edad:      %d\n", iter->edad);
        posicion++;
    }
}

/* Libera todos los nodos, incluyendo los centinelas. */
void destruir_lista(Lista *lista)
{
    Registro *actual;
    Registro *siguiente;

    actual = lista->Inicial;

    while (actual != NULL)
    {
        siguiente = actual->next;
        free(actual);
        actual = siguiente;
    }

    lista->Inicial = NULL;
    lista->Final = NULL;
    lista->len = 0;
}

int main(void)
{
    Lista pila;
    Registro extraido;

    printf("========================================\n");
    printf("       DEMOSTRACION DE PILA LIFO\n");
    printf("       Last In, First Out\n");
    printf("========================================\n\n");

    if (!crear_lista(&pila))
    {
        printf("Error: no se pudo crear la pila.\n");
        return 1;
    }

    printf("Pila creada. Size: %d\n\n", pila.len);

    /* PASO 1: inserciones */
    printf("PASO 1: Insertando Jose Ernesto...\n");
    agregar(&pila, "Jose Ernesto", "Cuaupec Barrio Alto", 36);
    printf("Size: %d\n\n", pila.len);

    printf("PASO 2: Insertando Alondra...\n");
    agregar(&pila, "Alondra", "Ticoman", 22);
    printf("Size: %d\n\n", pila.len);

    printf("PASO 3: Insertando Braulio...\n");
    agregar(&pila, "Braulio", "Ticoman", 26);
    printf("Size: %d\n\n", pila.len);

    printf("PASO 4: Insertando Francisco...\n");
    agregar(&pila, "Francisco", "La Pastora", 26);
    printf("Size: %d\n\n", pila.len);

    printf("----------------------------------------\n");
    printf("CONTENIDO DE LA PILA DESPUES DE INSERTAR\n");
    printf("----------------------------------------\n");
    print_list(&pila);

    /*
     * Orden de entrada:
     * Jose -> Alondra -> Braulio -> Francisco
     *
     * Por ser LIFO, el orden de salida debe ser:
     * Francisco -> Braulio -> Alondra -> Jose
     */
    printf("\n========================================\n");
    printf("EXTRACCIONES LIFO\n");
    printf("========================================\n");

    while (extraer_lifo(&pila, &extraido))
    {
        printf("\nSale de la pila:\n");
        printf("  Nombre:    %s\n", extraido.nombre);
        printf("  Direccion: %s\n", extraido.direccion);
        printf("  Edad:      %d\n", extraido.edad);
        printf("Elementos restantes: %d\n", pila.len);

        if (pila.len > 0)
        {
            printf("\nPila restante:\n");
            print_list(&pila);
        }
    }

    printf("\nIntentando extraer de una pila vacia...\n");
    if (!extraer_lifo(&pila, &extraido))
        printf("No se puede extraer: la pila esta vacia.\n");

    destruir_lista(&pila);

    printf("\nMemoria liberada correctamente.\n");

    return 0;
}
