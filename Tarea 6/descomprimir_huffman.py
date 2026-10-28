import heapq
import json
import struct


class NodoHuffman:

    def __init__(self, char, freq):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None

    def __lt__(self, other):
        return self.freq < other.freq


def construir_arbol(frecuencias):
    heap = []

    for char, freq in frecuencias.items():
        heapq.heappush(heap, NodoHuffman(char, freq))

    while len(heap) > 1:
        nodo1 = heapq.heappop(heap)
        nodo2 = heapq.heappop(heap)

        combinado = NodoHuffman(None, nodo1.freq + nodo2.freq)
        combinado.left = nodo1
        combinado.right = nodo2

        heapq.heappush(heap, combinado)

    return heap[0] if heap else None


def descomprimir(ruta_comprimido, ruta_salida):

    with open(ruta_comprimido, "rb") as archivo:

        longitud_cabecera = struct.unpack(
            ">I",
            archivo.read(4)
        )[0]

        cabecera = archivo.read(longitud_cabecera)

        frecuencias = json.loads(
            cabecera.decode("utf-8")
        )

        extra_padding = archivo.read(1)[0]

        datos = archivo.read()

    raiz = construir_arbol(frecuencias)

    bits = "".join(
        f"{byte:08b}"
        for byte in datos
    )

    if extra_padding > 0:
        bits = bits[:-extra_padding]

    texto_decodificado = []

    if raiz is not None and raiz.char is not None:
        texto_decodificado = [raiz.char] * raiz.freq

    else:
        nodo_actual = raiz

        for bit in bits:

            if bit == "0":
                nodo_actual = nodo_actual.left
            else:
                nodo_actual = nodo_actual.right

            if nodo_actual.char is not None:
                texto_decodificado.append(nodo_actual.char)
                nodo_actual = raiz

    resultado = "".join(texto_decodificado)

    with open(ruta_salida, "w", encoding="utf-8") as salida:
        salida.write(resultado)

    print(
        f"Archivo descomprimido exitosamente en: {ruta_salida}"
    )


if __name__ == "__main__":

    descomprimir(
        "CINVESTAV_Zacatenco.bin",
        "CINVESTAV_Zacatenco_recuperado.txt"
    )
