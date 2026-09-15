#include <stdio.h>
#include <stdint.h>
#include <immintrin.h> // Cabecera para Intel Intrinsics (_mulx_u64, _addcarry_u64)

// Estructura para representar un entero no firmado de 128 bits
typedef struct {
    uint64_t lo; // Bits bajos (0-63)
    uint64_t hi; // Bits altos (64-127)
} uint128_t;

// Estructura para almacenar el resultado completo de 256 bits
typedef struct {
    uint64_t r0; // Más significativo a menos significativo: r3 || r2 || r1 || r0
    uint64_t r1;
    uint64_t r2;
    uint64_t r3;
} uint256_t;

// Función para imprimir cualquier variable byte por byte junto con su dirección de memoria
void imprimir_memoria_bytes(const char *nombre_variable, void *ptr, size_t tamanio) {
    unsigned char *bytes = (unsigned char *)ptr;
    printf("--- Volcado de memoria: %s (%zu bytes) ---\n", nombre_variable, tamanio);
    for (size_t i = 0; i < tamanio; i++) {
        // Se muestra la dirección física del byte y su valor en hexadecimal
        printf("  Direccion: %p  ->  Byte[%2zu]: 0x%02X\n", (void*)&bytes[i], i, bytes[i]);
    }
    printf("\n");
}

// Multiplicación de 128 bits x 128 bits usando Intel Intrinsics (Corregida para GCC estricto)
uint256_t multiplicar_128bits(uint128_t A, uint128_t B) {
    uint256_t R = {0};
    
    uint64_t low1, high1;
    uint64_t low2, high2;
    uint64_t low3, high3;
    uint64_t low4, high4;
    
    // 1. Productos parciales usando MULX (no altera banderas aritméticas)
    low1 = _mulx_u64(A.lo, B.lo, (unsigned long long *)&high1); // A.lo * B.lo
    low2 = _mulx_u64(A.lo, B.hi, (unsigned long long *)&high2); // A.lo * B.hi
    low3 = _mulx_u64(A.hi, B.lo, (unsigned long long *)&high3); // A.hi * B.lo
    low4 = _mulx_u64(A.hi, B.hi, (unsigned long long *)&high4); // A.hi * B.hi

    // Columna 0
    R.r0 = low1;

    // Columna 1: Acumulación con propagación de acarreos
    uint8_t acarreo = 0;
    acarreo = _addcarry_u64(0,       high1, low2, (unsigned long long *)&R.r1);
    acarreo = _addcarry_u64(acarreo, R.r1,  low3, (unsigned long long *)&R.r1);

    // Columna 2
    uint64_t suma_interna_col2;
    uint8_t acarreo_col2 = _addcarry_u64(0,           high2, high3, (unsigned long long *)&suma_interna_col2);
    acarreo_col2         = _addcarry_u64(acarreo_col2, suma_interna_col2, low4, (unsigned long long *)&suma_interna_col2);
    
    // Sumamos el acarreo proveniente de la Columna 1
    acarreo = _addcarry_u64(acarreo, suma_interna_col2, 0, (unsigned long long *)&R.r2);

    // Columna 3: Recolección final de los límites superiores
    R.r3 = high4 + acarreo_col2 + acarreo;

    return R;
}

int main() {
    // Ejemplo de valores de entrada (128 bits cada uno)
    uint128_t A = {0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL};
    uint128_t B = {0x0000000000000002ULL, 0x0000000000000002ULL};

    // Ejecución del cálculo optimizado
    uint256_t resultado = multiplicar_128bits(A, B);

    // Impresión de resultados en consola para validación aritmética
    printf("Operacion finalizada.\n");
    printf("Resultado esperado en Hex (256-bit):\n");
    printf("R3: %016llX | R2: %016llX | R1: %016llX | R0: %016llX\n\n", 
            (unsigned long long)resultado.r3, (unsigned long long)resultado.r2, 
            (unsigned long long)resultado.r1, (unsigned long long)resultado.r0);

    // Imprimir las direcciones de memoria como bytes individuales
    imprimir_memoria_bytes("Operando A (128 bits)", &A, sizeof(A));
    imprimir_memoria_bytes("Operando B (128 bits)", &B, sizeof(B));
    imprimir_memoria_bytes("Resultado R (256 bits)", &resultado, sizeof(resultado));

    return 0;
}
