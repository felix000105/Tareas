# cython: language_level=3

import heapq
import json
import struct
from collections import Counter


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


def generar_codigos(nodo, codigo_actual="", codigos=None):
    if codigos is None:
        codigos = {}

    if nodo is None:
        return codigos

    if nodo.simbolo is not None:
        if codigo_actual:
            codigos[nodo.simbolo] = codigo_actual
        else:
            codigos[nodo.simbolo] = "0"
        return codigos

    generar_codigos(nodo.left, codigo_actual + "0", codigos)
    generar_codigos(nodo.right, codigo_actual + "1", codigos)
    return codigos


def representar_simbolo(simbolo):
    if simbolo == " ":
        return "' '"
    if simbolo == "\n":
        return "'\\n'"
    if simbolo == "\t":
        return "'\\t'"
    return repr(simbolo)


def imprimir_arbol(nodo, prefijo="", rama="RAIZ"):
    if nodo is None:
        return

    if nodo.simbolo is None:
        print(f"{prefijo}{rama}: * ({nodo.freq})")
    else:
        print(f"{prefijo}{rama}: {representar_simbolo(nodo.simbolo)} ({nodo.freq})")

    if nodo.left is not None:
        imprimir_arbol(nodo.left, prefijo + "    ", "0")
    if nodo.right is not None:
        imprimir_arbol(nodo.right, prefijo + "    ", "1")


cpdef comprimir(str ruta_entrada, str ruta_salida):
    cdef str texto
    cdef dict frecuencias
    cdef dict codigos
    cdef object raiz
    cdef list partes
    cdef str simbolo
    cdef str texto_codificado
    cdef int extra_padding
    cdef bytearray bytes_comprimidos
    cdef Py_ssize_t i
    cdef str bloque
    cdef bytes cabecera

    with open(ruta_entrada, "r", encoding="utf-8") as archivo:
        texto = archivo.read()

    if not texto:
        raise ValueError("El archivo de entrada esta vacio.")

    frecuencias = dict(Counter(texto))
    raiz = construir_arbol(frecuencias)
    codigos = generar_codigos(raiz)

    partes = []
    for simbolo in texto:
        partes.append(codigos[simbolo])
    texto_codificado = "".join(partes)

    extra_padding = (8 - len(texto_codificado) % 8) % 8
    if extra_padding > 0:
        texto_codificado += "0" * extra_padding

    bytes_comprimidos = bytearray()
    for i in range(0, len(texto_codificado), 8):
        bloque = texto_codificado[i:i + 8]
        bytes_comprimidos.append(int(bloque, 2))

    cabecera = json.dumps(
        frecuencias,
        ensure_ascii=False
    ).encode("utf-8")

    with open(ruta_salida, "wb") as salida:
        salida.write(struct.pack(">I", len(cabecera)))
        salida.write(cabecera)
        salida.write(bytes([extra_padding]))
        salida.write(bytes_comprimidos)

    print("\nCODIGOS HUFFMAN:\n")
    for simbolo, codigo in codigos.items():
        print(f"{representar_simbolo(simbolo):>8} -> {codigo}")

    print("\nARBOL HUFFMAN:\n")
    imprimir_arbol(raiz)

    print(f"\nArchivo comprimido exitosamente en: {ruta_salida}")
    return codigos


def main():
    comprimir(
        "CINVESTAV_Zacatenco.txt",
        "CINVESTAV_Zacatenco.bin"
    )
