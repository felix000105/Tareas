from setuptools import setup
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(
        [
            "comprimir_huffman.pyx",
            "descomprimir_huffman.pyx"
        ],
        compiler_directives={"language_level": "3"}
    )
)
