# cython: language_level=3

import heapq
import json
import struct


cdef class NodoHuffman:
    cdef public object simbolo
    cdef public unsigned long long freq
    cdef public object left
    cdef public object right

    def __init__(self, simbolo, freq):
        self.simbolo = simbolo
        self.freq = freq
        self.left = None
        self.right = None

    def __lt__(self, other):
        return self.freq < other.freq


def construir_arbol(frecuencias):
    heap = []

    for simbolo, freq in frecuencias.items():
        heapq.heappush(heap, NodoHuffman(simbolo, freq))

    while len(heap) > 1:
        nodo1 = heapq.heappop(heap)
        nodo2 = heapq.heappop(heap)

        combinado = NodoHuffman(None, nodo1.freq + nodo2.freq)
        combinado.left = nodo1
        combinado.right = nodo2

        heapq.heappush(heap, combinado)

    if heap:
        return heap[0]
    return None


cpdef str descomprimir(str ruta_comprimido, str ruta_salida):
    cdef bytes longitud_bytes
    cdef unsigned int longitud_cabecera
    cdef bytes cabecera
    cdef dict frecuencias
    cdef bytes padding_byte
    cdef int extra_padding
    cdef bytes datos
    cdef object raiz
    cdef list partes_bits
    cdef object valor_byte
    cdef str bits
    cdef list texto_decodificado
    cdef object nodo_actual
    cdef str bit
    cdef str resultado

    with open(ruta_comprimido, "rb") as archivo:
        longitud_bytes = archivo.read(4)
        if len(longitud_bytes) != 4:
            raise ValueError("Archivo comprimido invalido.")

        longitud_cabecera = struct.unpack(">I", longitud_bytes)[0]
        cabecera = archivo.read(longitud_cabecera)
        frecuencias = json.loads(cabecera.decode("utf-8"))

        padding_byte = archivo.read(1)
        if len(padding_byte) != 1:
            raise ValueError("Archivo comprimido invalido: falta padding.")

        extra_padding = padding_byte[0]
        datos = archivo.read()

    raiz = construir_arbol(frecuencias)

    if raiz is None:
        resultado = ""
    elif raiz.simbolo is not None:
        resultado = raiz.simbolo * raiz.freq
    else:
        partes_bits = []
        for valor_byte in datos:
            partes_bits.append(format(valor_byte, "08b"))
        bits = "".join(partes_bits)

        if extra_padding > 0:
            bits = bits[:-extra_padding]

        texto_decodificado = []
        nodo_actual = raiz

        for bit in bits:
            if bit == "0":
                nodo_actual = nodo_actual.left
            else:
                nodo_actual = nodo_actual.right

            if nodo_actual.simbolo is not None:
                texto_decodificado.append(nodo_actual.simbolo)
                nodo_actual = raiz

        resultado = "".join(texto_decodificado)

    with open(ruta_salida, "w", encoding="utf-8") as salida:
        salida.write(resultado)

    print(f"Archivo descomprimido exitosamente en: {ruta_salida}")
    return resultado


def main():
    descomprimir(
        "CINVESTAV_Zacatenco.bin",
        "CINVESTAV_Zacatenco_recuperado.txt"
    )
