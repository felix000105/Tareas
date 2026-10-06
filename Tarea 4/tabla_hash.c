#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TABLE_SIZE 16

typedef struct Node {
    uint32_t key;
    int value;
    struct Node *next;
} Node;

typedef struct {
    Node *buckets[TABLE_SIZE];
} HashTable;

uint32_t hash_function(uint32_t key) {
    return key % TABLE_SIZE;
}

void init_table(HashTable *table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
}

int insert(HashTable *table, uint32_t key, int value) {
    uint32_t index = hash_function(key);

    Node *current = table->buckets[index];
    while (current != NULL) {
        if (current->key == key && current->value == value) {
            return 0;
        }
        current = current->next;
    }

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        return -1;
    }

    new_node->key = key;
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;

    return 1;
}

int search_exact(const HashTable *table, uint32_t key, int value) {
    uint32_t index = hash_function(key);
    const Node *current = table->buckets[index];

    while (current != NULL) {
        if (current->key == key && current->value == value) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

void print_table(const HashTable *table) {
    printf("\nContenido de la Hash Table:\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("[%d]", i);

        const Node *current = table->buckets[i];
        if (current != NULL) {
            printf(" -> ");
            while (current != NULL) {
                printf("(%u, %d)", (unsigned int)current->key, current->value);
                if (current->next != NULL) {
                    printf(" -> ");
                }
                current = current->next;
            }
        }

        printf("\n");
    }
}

void free_table(HashTable *table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *current = table->buckets[i];

        while (current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }

        table->buckets[i] = NULL;
    }
}

int main(void) {
    HashTable ht;
    init_table(&ht);

    if (insert(&ht, 0xA1B2, 100) == -1 ||
        insert(&ht, 0xA1B2, 200) == -1 ||
        insert(&ht, 0xA1B2, 300) == -1 ||
        insert(&ht, 0xA1B2, 100) == -1 ||
        insert(&ht, 0xFFFFFFFF, 400) == -1 ||
        insert(&ht, 42, 500) == -1) {
        fprintf(stderr, "Error reservando memoria.\n");
        free_table(&ht);
        return 1;
    }

    print_table(&ht);

    printf("\nBusquedas:\n");

    uint32_t clave_buscar = 0xA1B2;
    int valores_a_probar[] = {100, 200, 300, 999};
    int cantidad_valores = (int)(sizeof(valores_a_probar) / sizeof(valores_a_probar[0]));

    for (int i = 0; i < cantidad_valores; i++) {
        if (search_exact(&ht, clave_buscar, valores_a_probar[i])) {
            printf("Clave %u con valor %d existe.\n",
                   (unsigned int)clave_buscar, valores_a_probar[i]);
        } else {
            printf("Clave %u con valor %d NO existe.\n",
                   (unsigned int)clave_buscar, valores_a_probar[i]);
        }
    }

    free_table(&ht);

    return 0;
}
