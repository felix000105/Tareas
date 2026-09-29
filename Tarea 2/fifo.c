#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * FIFO (First In, First Out)
 * Basado en la lista doblemente ligada del programa lista_ligada.c.
 *
 * Insercion: al final de la lista.
 * Extraccion: al inicio de la lista.
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
 * Esta operacion corresponde al ENQUEUE de una cola FIFO.
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
 * Extrae el PRIMER elemento que fue agregado.
 * FIFO: First In, First Out.
 *
 * El registro extraido se copia en *dato.
 * Regresa 1 si se extrajo correctamente y 0 si la cola esta vacia.
 */
int extraer_fifo(Lista *lista, Registro *dato)
{
    Registro *primero;

    if (lista->len == 0)
        return 0;

    primero = lista->Inicial->next;

    strcpy(dato->nombre, primero->nombre);
    strcpy(dato->direccion, primero->direccion);
    dato->edad = primero->edad;
    dato->next = NULL;
    dato->prev = NULL;

    lista->Inicial->next = primero->next;
    primero->next->prev = lista->Inicial;

    free(primero);
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
        printf("La cola esta vacia.\n");
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
    Lista cola;
    Registro extraido;

    printf("========================================\n");
    printf("       DEMOSTRACION DE COLA FIFO\n");
    printf("       First In, First Out\n");
    printf("========================================\n\n");

    if (!crear_lista(&cola))
    {
        printf("Error: no se pudo crear la cola.\n");
        return 1;
    }

    printf("Cola creada. Size: %d\n\n", cola.len);

    /* PASO 1: inserciones */
    printf("PASO 1: Insertando Jose Ernesto...\n");
    agregar(&cola, "Jose Ernesto", "Cuaupec Barrio Alto", 36);
    printf("Size: %d\n\n", cola.len);

    printf("PASO 2: Insertando Alondra...\n");
    agregar(&cola, "Alondra", "Ticoman", 22);
    printf("Size: %d\n\n", cola.len);

    printf("PASO 3: Insertando Braulio...\n");
    agregar(&cola, "Braulio", "Ticoman", 26);
    printf("Size: %d\n\n", cola.len);

    printf("PASO 4: Insertando Francisco...\n");
    agregar(&cola, "Francisco", "La Pastora", 26);
    printf("Size: %d\n\n", cola.len);

    printf("----------------------------------------\n");
    printf("CONTENIDO DE LA COLA DESPUES DE INSERTAR\n");
    printf("----------------------------------------\n");
    print_list(&cola);

    /*
     * Orden de entrada:
     * Jose -> Alondra -> Braulio -> Francisco
     *
     * Por ser FIFO, el orden de salida debe ser exactamente el mismo.
     */
    printf("\n========================================\n");
    printf("EXTRACCIONES FIFO\n");
    printf("========================================\n");

    while (extraer_fifo(&cola, &extraido))
    {
        printf("\nSale de la cola:\n");
        printf("  Nombre:    %s\n", extraido.nombre);
        printf("  Direccion: %s\n", extraido.direccion);
        printf("  Edad:      %d\n", extraido.edad);
        printf("Elementos restantes: %d\n", cola.len);

        if (cola.len > 0)
        {
            printf("\nCola restante:\n");
            print_list(&cola);
        }
    }

    printf("\nIntentando extraer de una cola vacia...\n");
    if (!extraer_fifo(&cola, &extraido))
        printf("No se puede extraer: la cola esta vacia.\n");

    destruir_lista(&cola);

    printf("\nMemoria liberada correctamente.\n");

    return 0;
}
