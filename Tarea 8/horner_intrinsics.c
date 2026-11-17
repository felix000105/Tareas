#define _POSIX_C_SOURCE 200809L

#include <immintrin.h>
#include <stddef.h>
#include <time.h>

float horner_scalar(float x, const float *coef, size_t size) {
    float acc = 0.0f;

    for (size_t i = 0; i < size; ++i) {
        acc = (acc + coef[i]) * x;
    }

    return acc;
}

float horner_avx(float x, const float *coef, size_t size) {
    if (size < 8 || (size % 8) != 0) {
        return horner_scalar(x, coef, size);
    }

    float x2 = x * x;
    float x4 = x2 * x2;
    float x8 = x4 * x4;

    __m256 vx8 = _mm256_set1_ps(x8);
    __m256 y = _mm256_setzero_ps();

    size_t bloques = size / 8;

    for (size_t i = 0; i + 1 < bloques; ++i) {
        __m256 c = _mm256_loadu_ps(coef + 8 * i);
        y = _mm256_add_ps(y, c);
        y = _mm256_mul_ps(y, vx8);
    }

    __m256 c = _mm256_loadu_ps(coef + 8 * (bloques - 1));
    y = _mm256_add_ps(y, c);

    float r[8];
    _mm256_storeu_ps(r, y);

    float p = 0.0f;
    float potencia = x;

    for (int i = 7; i >= 0; --i) {
        p += r[i] * potencia;
        potencia *= x;
    }

    return p;
}

int avx_disponible(void) {
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    __builtin_cpu_init();
    return __builtin_cpu_supports("avx") ? 1 : 0;
#else
    return 1;
#endif
}

static double segundos(const struct timespec *inicio,
                       const struct timespec *fin) {
    return (double)(fin->tv_sec - inicio->tv_sec) +
           (double)(fin->tv_nsec - inicio->tv_nsec) / 1.0e9;
}

double benchmark_scalar(float x,
                        const float *coef,
                        size_t size,
                        int repeticiones) {
    struct timespec inicio, fin;
    volatile float vx = x;
    volatile float sink = 0.0f;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < repeticiones; ++i) {
        sink = horner_scalar(vx, coef, size);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);

    (void)sink;
    return segundos(&inicio, &fin);
}

double benchmark_avx(float x,
                     const float *coef,
                     size_t size,
                     int repeticiones) {
    struct timespec inicio, fin;
    volatile float vx = x;
    volatile float sink = 0.0f;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < repeticiones; ++i) {
        sink = horner_avx(vx, coef, size);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);

    (void)sink;
    return segundos(&inicio, &fin);
}
