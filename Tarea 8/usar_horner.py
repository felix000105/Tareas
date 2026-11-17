import ctypes
import random
import subprocess
from pathlib import Path

BASE = Path(__file__).resolve().parent
FUENTE_C = BASE / "horner_intrinsics.c"
LIBRERIA = BASE / "libhorner.so"


def compilar_libreria():
    comando = [
        "gcc",
        "-O3",
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-mavx",
        "-fPIC",
        "-shared",
        str(FUENTE_C),
        "-o",
        str(LIBRERIA),
    ]

    subprocess.run(comando, check=True)


def cargar_libreria():
    if not LIBRERIA.exists():
        compilar_libreria()

    lib = ctypes.CDLL(str(LIBRERIA))
    ptr_float = ctypes.POINTER(ctypes.c_float)

    lib.horner_scalar.argtypes = [
        ctypes.c_float,
        ptr_float,
        ctypes.c_size_t,
    ]
    lib.horner_scalar.restype = ctypes.c_float

    lib.horner_avx.argtypes = [
        ctypes.c_float,
        ptr_float,
        ctypes.c_size_t,
    ]
    lib.horner_avx.restype = ctypes.c_float

    lib.avx_disponible.argtypes = []
    lib.avx_disponible.restype = ctypes.c_int

    lib.benchmark_scalar.argtypes = [
        ctypes.c_float,
        ptr_float,
        ctypes.c_size_t,
        ctypes.c_int,
    ]
    lib.benchmark_scalar.restype = ctypes.c_double

    lib.benchmark_avx.argtypes = [
        ctypes.c_float,
        ptr_float,
        ctypes.c_size_t,
        ctypes.c_int,
    ]
    lib.benchmark_avx.restype = ctypes.c_double

    return lib


def main():
    lib = cargar_libreria()

    if not lib.avx_disponible():
        raise RuntimeError("El procesador no reporta soporte para AVX.")

    size = 10000
    x = 0.999
    repeticiones = 10000

    rng = random.Random(12345)

    Coeficientes = ctypes.c_float * size
    coeficientes = Coeficientes(
        *[rng.randrange(1000) / 1000.0 for _ in range(size)]
    )

    escalar = lib.horner_scalar(
        ctypes.c_float(x),
        coeficientes,
        size,
    )

    avx = lib.horner_avx(
        ctypes.c_float(x),
        coeficientes,
        size,
    )

    error_abs = abs(float(escalar) - float(avx))
    referencia = max(abs(float(escalar)), 1.0)
    error_rel = error_abs / referencia

    tiempo_escalar = lib.benchmark_scalar(
        ctypes.c_float(x),
        coeficientes,
        size,
        repeticiones,
    )

    tiempo_avx = lib.benchmark_avx(
        ctypes.c_float(x),
        coeficientes,
        size,
        repeticiones,
    )

    print(f"Resultado escalar : {escalar:.9f}")
    print(f"Resultado AVX     : {avx:.9f}")
    print(f"Error absoluto    : {error_abs:.9e}")
    print(f"Error relativo    : {error_rel:.9e}")
    print()
    print(f"Tiempo escalar    : {tiempo_escalar:.6f} s")
    print(f"Tiempo AVX        : {tiempo_avx:.6f} s")

    if tiempo_avx > 0.0:
        print(f"Speedup AVX       : {tiempo_escalar / tiempo_avx:.3f}x")


if __name__ == "__main__":
    main()
