import heapq
import json
import struct
from collections import Counter


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


def generar_codigos(nodo, codigo_actual="", codigos=None):

    if codigos is None:
        codigos = {}

    if nodo is None:
        return codigos

    if nodo.char is not None:
        codigos[nodo.char] = codigo_actual if codigo_actual else "0"
        return codigos

    generar_codigos(nodo.left, codigo_actual + "0", codigos)
    generar_codigos(nodo.right, codigo_actual + "1", codigos)

    return codigos


def representar_char(char):

    if char == " ":
        return "' '"

    if char == "\n":
        return "'\\n'"

    if char == "\t":
        return "'\\t'"

    return repr(char)


def imprimir_arbol(nodo, prefijo="", rama="RAIZ"):

    if nodo is None:
        return

    if nodo.char is None:
        print(f"{prefijo}{rama}: * ({nodo.freq})")
    else:
        print(f"{prefijo}{rama}: {representar_char(nodo.char)} ({nodo.freq})")

    if nodo.left is not None:
        imprimir_arbol(nodo.left, prefijo + "    ", "0")

    if nodo.right is not None:
        imprimir_arbol(nodo.right, prefijo + "    ", "1")


def comprimir(ruta_entrada, ruta_salida):

    with open(ruta_entrada, "r", encoding="utf-8") as archivo:
        texto = archivo.read()

    if not texto:
        print("El archivo está vacío.")
        return

    frecuencias = Counter(texto)

    raiz = construir_arbol(frecuencias)
    codigos = generar_codigos(raiz)

    texto_codificado = "".join(codigos[char] for char in texto)

    extra_padding = (8 - len(texto_codificado) % 8) % 8
    texto_codificado += "0" * extra_padding

    bytes_comprimidos = bytearray()

    for i in range(0, len(texto_codificado), 8):
        byte = texto_codificado[i:i + 8]
        bytes_comprimidos.append(int(byte, 2))

    cabecera = json.dumps(
        dict(frecuencias),
        ensure_ascii=False
    ).encode("utf-8")

    with open(ruta_salida, "wb") as salida:
        salida.write(struct.pack(">I", len(cabecera)))
        salida.write(cabecera)
        salida.write(bytes([extra_padding]))
        salida.write(bytes_comprimidos)

    print("\nCODIGOS HUFFMAN:\n")

    for char, codigo in sorted(
        codigos.items(),
        key=lambda elemento: (len(elemento[1]), elemento[1])
    ):
        print(f"{representar_char(char):>8} -> {codigo}")

    print("\nARBOL HUFFMAN:\n")
    imprimir_arbol(raiz)

    print(f"\nArchivo comprimido exitosamente en: {ruta_salida}")


if __name__ == "__main__":

    comprimir(
        "CINVESTAV_Zacatenco.txt",
        "CINVESTAV_Zacatenco.bin"
    )
