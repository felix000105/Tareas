import heapq
import os
from collections import Counter


class NodoHuffman:

  def __init__(self, char, freq):
    self.char = char
    self.freq = freq
    self.left = None
    self.right = None

  def __lt__(self, other):
    return self.freq < other.freq


class CompresorHuffman:

  def __init__(self, ruta_archivo):
    self.ruta_archivo = ruta_archivo
    self.heap = []
    self.codigos = {}
    self.inverso_codigos = {}

  def _calcular_frecuencias(self, texto):
    return Counter(texto)

  def _construir_arbol(self, frecuencias):
    for char, freq in frecuencias.items():
      nodo = NodoHuffman(char, freq)
      heapq.heappush(self.heap, nodo)

    while len(self.heap) > 1:
      nodo1 = heapq.heappop(self.heap)
      nodo2 = heapq.heappop(self.heap)

      combinado = NodoHuffman(None, nodo1.freq + nodo2.freq)
      combinado.left = nodo1
      combinado.right = nodo2

      heapq.heappush(self.heap, combinado)

    return self.heap[0] if self.heap else None

  def _generar_codigos_rec(self, nodo, codigo_actual):
    if nodo is None:
      return

    if nodo.char is not None:
      self.codigos[nodo.char] = codigo_actual
      self.inverso_codigos[codigo_actual] = nodo.char
      return

    self._generar_codigos_rec(nodo.left, codigo_actual + "0")
    self._generar_codigos_rec(nodo.right, codigo_actual + "1")

  def _pad_encoded_text(self, encoded_text):
    extra_padding = 8 - len(encoded_text) % 8
    encoded_text += "0" * extra_padding
    padded_info = "{0:08b}".format(extra_padding)
    return padded_info + encoded_text

  def _get_byte_array(self, padded_encoded_text):
    if len(padded_encoded_text) % 8 != 0:
      raise ValueError("El texto codificado no está alineado a bytes.")
    b = bytearray()
    for i in range(0, len(padded_encoded_text), 8):
      byte = padded_encoded_text[i : i + 8]
      b.append(int(byte, 2))
    return b

  def comprimir(self, ruta_salida):
    with open(self.ruta_archivo, "r", encoding="utf-8") as f:
      texto = f.read()

    frecuencias = self._calcular_frecuencias(texto)
    raiz = self._construir_arbol(frecuencias)
    self._generar_codigos_rec(raiz, "")

    texto_codificado = "".join([self.codigos[char] for char in texto])
    texto_relleno = self._pad_encoded_text(texto_codificado)
    bytes_salida = self._get_byte_array(texto_relleno)

    with open(ruta_salida, "wb") as salida:
      salida.write(bytes_salida)

    print(f"Archivo comprimido exitosamente en: {ruta_salida}")
    return self.codigos, raiz

  def _remover_padding(self, padded_encoded_text):
    padded_info = padded_encoded_text[:8]
    extra_padding = int(padded_info, 2)

    texto_codificado = padded_encoded_text[8:]
    texto_codificado = texto_codificado[
        :-extra_padding if extra_padding > 0 else None
    ]
    return texto_codificado

  def descomprimir(self, ruta_comprimido, raiz_arbol, ruta_salida):
    with open(ruta_comprimido, "rb") as f:
      bits = []
      byte = f.read(1)
      while byte:
        byte_val = ord(byte)
        bits.append("{0:08b}".format(byte_val))
        byte = f.read(1)

    texto_bits = "".join(bits)
    texto_codificado = self._remover_padding(texto_bits)

    texto_decodificado = []
    nodo_actual = raiz_arbol

    for bit in texto_codificado:
      if bit == "0":
        nodo_actual = nodo_actual.left
      else:
        nodo_actual = nodo_actual.right

      if nodo_actual.char is not None:
        texto_decodificado.append(nodo_actual.char)
        nodo_actual = raiz_arbol

    resultado = "".join(texto_decodificado)
    with open(ruta_salida, "w", encoding="utf-8") as salida:
      salida.write(resultado)

    print(f"Archivo descomprimido exitosamente en: {ruta_salida}")
    return resultado


def representar_char(char):
  if char == " ":
    return "' '"
  if char == "\n":
    return "'\\n'"
  if char == "\t":
    return "'\\t'"
  return repr(char)


def imprimir_arbol(nodo, prefijo="", rama="RAIZ", lineas=None):
  if lineas is None:
    lineas = []

  if nodo is None:
    return lineas

  if nodo.char is None:
    etiqueta = f"{rama}: * ({nodo.freq})"
  else:
    etiqueta = f"{rama}: {representar_char(nodo.char)} ({nodo.freq})"

  lineas.append(prefijo + etiqueta)

  if nodo.left is not None or nodo.right is not None:
    imprimir_arbol(nodo.left, prefijo + "    ", "0", lineas)
    imprimir_arbol(nodo.right, prefijo + "    ", "1", lineas)

  return lineas


if __name__ == "__main__":
  archivo_original = "CINVESTAV_Zacatenco.txt"
  archivo_comprimido = "CINVESTAV_Zacatenco.bin"
  archivo_recuperado = "CINVESTAV_Zacatenco_recuperado.txt"
  archivo_arbol = "arbol_huffman.txt"

  compresor = CompresorHuffman(archivo_original)

  codigos, raiz = compresor.comprimir(archivo_comprimido)

  print("\nCODIGOS HUFFMAN:")
  for char, codigo in sorted(codigos.items(), key=lambda x: (len(x[1]), x[1])):
    print(f"{representar_char(char):>8} -> {codigo}")

  print("\nARBOL HUFFMAN:")
  lineas_arbol = imprimir_arbol(raiz)
  for linea in lineas_arbol:
    print(linea)

  with open(archivo_arbol, "w", encoding="utf-8") as f:
    f.write("\n".join(lineas_arbol))

  compresor.descomprimir(archivo_comprimido, raiz, archivo_recuperado)

  with open(archivo_original, "r", encoding="utf-8") as f:
    original = f.read()

  with open(archivo_recuperado, "r", encoding="utf-8") as f:
    recuperado = f.read()

  print("\nVERIFICACION:")
  print("Original y recuperado son iguales:", original == recuperado)

  tam_original = os.path.getsize(archivo_original)
  tam_comprimido = os.path.getsize(archivo_comprimido)

  print(f"Tamaño original: {tam_original} bytes")
  print(f"Tamaño comprimido: {tam_comprimido} bytes")
  if tam_original > 0:
    print(f"Relación comprimido/original: {tam_comprimido / tam_original:.4f}")
    print(f"Ahorro de espacio: {(1 - tam_comprimido / tam_original) * 100:.2f}%")
