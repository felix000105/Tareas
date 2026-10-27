#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define POP_SIZE 50
#define GENERATIONS 1000
#define CROSSOVER_RATE 0.90
#define MUTATION_RATE 0.01
#define LOWER_BOUND -5.12
#define UPPER_BOUND 5.12
#define PI 3.14159265358979323846

typedef struct {
    uint64_t xy;
    double x;
    double y;
    double fitness;
} Individual;

static uint32_t random_uint32(void) {
    uint32_t a = (uint32_t)(rand() & 0xFFFF);
    uint32_t b = (uint32_t)(rand() & 0xFFFF);
    return (a << 16) | b;
}

static uint64_t create_chromosome(uint32_t x_bits, uint32_t y_bits) {
    return ((uint64_t)x_bits << 32) | (uint64_t)y_bits;
}

static uint32_t get_x_bits(uint64_t xy) {
    return (uint32_t)(xy >> 32);
}

static uint32_t get_y_bits(uint64_t xy) {
    return (uint32_t)(xy & 0xFFFFFFFFULL);
}

static double decode(uint32_t value) {
    return LOWER_BOUND + ((double)value / (double)UINT32_MAX) * (UPPER_BOUND - LOWER_BOUND);
}

static double rastrigin(double x, double y) {
    return 20.0 + x * x + y * y
           - 10.0 * (cos(2.0 * PI * x) + cos(2.0 * PI * y));
}

static void evaluate(Individual *ind) {
    uint32_t x_bits = get_x_bits(ind->xy);
    uint32_t y_bits = get_y_bits(ind->xy);

    ind->x = decode(x_bits);
    ind->y = decode(y_bits);
    ind->fitness = rastrigin(ind->x, ind->y);
}

static void initialize_population(Individual population[POP_SIZE]) {
    for (int i = 0; i < POP_SIZE; i++) {
        uint32_t x_bits = random_uint32();
        uint32_t y_bits = random_uint32();

        population[i].xy = create_chromosome(x_bits, y_bits);
        evaluate(&population[i]);
    }
}

static int compare_individuals(const void *a, const void *b) {
    const Individual *ia = (const Individual *)a;
    const Individual *ib = (const Individual *)b;

    if (ia->fitness < ib->fitness) return -1;
    if (ia->fitness > ib->fitness) return 1;
    return 0;
}

static double random_01(void) {
    return (double)rand() / ((double)RAND_MAX + 1.0);
}

static void crossover(uint64_t parent1, uint64_t parent2,
                      uint64_t *child1, uint64_t *child2) {
    const uint64_t upper_mask = 0xFFFFFFFF00000000ULL;
    const uint64_t lower_mask = 0x00000000FFFFFFFFULL;

    if (random_01() < CROSSOVER_RATE) {
        *child1 = (parent1 & upper_mask) | (parent2 & lower_mask);
        *child2 = (parent2 & upper_mask) | (parent1 & lower_mask);
    } else {
        *child1 = parent1;
        *child2 = parent2;
    }
}

static void mutate(uint64_t *chromosome) {
    if (random_01() < MUTATION_RATE) {
        unsigned int bit = (unsigned int)(rand() % 64);
        uint64_t mask = 1ULL << bit;
        *chromosome ^= mask;
    }
}

static void generate_children(const Individual population[POP_SIZE],
                              Individual children[POP_SIZE]) {
    for (int i = 0; i < POP_SIZE; i += 2) {
        int p1 = rand() % POP_SIZE;
        int p2 = rand() % POP_SIZE;

        while (p2 == p1) {
            p2 = rand() % POP_SIZE;
        }

        uint64_t child1;
        uint64_t child2;

        crossover(population[p1].xy, population[p2].xy, &child1, &child2);

        mutate(&child1);
        mutate(&child2);

        children[i].xy = child1;
        evaluate(&children[i]);

        if (i + 1 < POP_SIZE) {
            children[i + 1].xy = child2;
            evaluate(&children[i + 1]);
        }
    }
}

static void select_best(Individual population[POP_SIZE],
                        const Individual children[POP_SIZE]) {
    Individual combined[POP_SIZE * 2];

    for (int i = 0; i < POP_SIZE; i++) {
        combined[i] = population[i];
        combined[i + POP_SIZE] = children[i];
    }

    qsort(combined, POP_SIZE * 2, sizeof(Individual), compare_individuals);

    for (int i = 0; i < POP_SIZE; i++) {
        population[i] = combined[i];
    }
}

static void print_binary64(uint64_t value) {
    for (int i = 63; i >= 0; i--) {
        putchar((value & (1ULL << i)) ? '1' : '0');
        if (i == 32) putchar('|');
    }
}

int main(void) {
    Individual population[POP_SIZE];
    Individual children[POP_SIZE];

    srand((unsigned int)time(NULL));

    initialize_population(population);
    qsort(population, POP_SIZE, sizeof(Individual), compare_individuals);

    printf("Generacion 0: f(x,y) = %.12f, x = %.12f, y = %.12f\n",
           population[0].fitness, population[0].x, population[0].y);

    for (int generation = 1; generation <= GENERATIONS; generation++) {
        generate_children(population, children);
        select_best(population, children);

        if (generation % 100 == 0 || generation == GENERATIONS) {
            printf("Generacion %d: f(x,y) = %.12f, x = %.12f, y = %.12f\n",
                   generation,
                   population[0].fitness,
                   population[0].x,
                   population[0].y);
        }
    }

    qsort(population, POP_SIZE, sizeof(Individual), compare_individuals);

    printf("\nMejor individuo:\n");
    printf("xy = ");
    print_binary64(population[0].xy);
    printf("\n");
    printf("x = %.12f\n", population[0].x);
    printf("y = %.12f\n", population[0].y);
    printf("Rastrigin(x,y) = %.12f\n", population[0].fitness);

    return 0;
}
