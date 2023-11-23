import ctypes
import concurrent.futures
import sys
import time
import os

size = None
num_pairs = None

# Define la estructura Matrices que coincida con la estructura en C
class Matrices(ctypes.Structure):
    _fields_ = [
        ("a", ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
        ("b", ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
        ("c", ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
        ("size", ctypes.c_int)
    ]

# Cargar la biblioteca compartida
lib = ctypes.CDLL('./libmatrixmul.so')

# Definir los tipos de argumentos y retorno para las funciones de C
lib.multiply_matrices.argtypes = [
    ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),
    ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),
    ctypes.POINTER(ctypes.POINTER(ctypes.c_double)),
    ctypes.c_int
]
lib.multiply_matrices.restype = None

lib.allocate_matrix.argtypes = [ctypes.c_int]
lib.allocate_matrix.restype = ctypes.POINTER(ctypes.POINTER(ctypes.c_double))

lib.free_matrix.argtypes = [ctypes.POINTER(ctypes.POINTER(ctypes.c_double)), ctypes.c_int]
lib.free_matrix.restype = None

# Función auxiliar para convertir listas de Python a matrices de C
def to_c_matrix(matrix, size):
    c_matrix = lib.allocate_matrix(size)
    for i, row in enumerate(matrix):
        for j, val in enumerate(row):
            c_matrix[i][j] = val
    return c_matrix

# Función para liberar la matriz de C
def free_c_matrix(c_matrix, size):
    lib.free_matrix(c_matrix, size)

# Función para multiplicar matrices
def multiply_matrices(a, b, size):
    a_c = to_c_matrix(a, size)
    b_c = to_c_matrix(b, size)
    c_c = lib.allocate_matrix(size)

    lib.multiply_matrices(a_c, b_c, c_c, ctypes.c_int(size))

    result = [[c_c[i][j] for j in range(size)] for i in range(size)]

    free_c_matrix(a_c, size)
    free_c_matrix(b_c, size)
    free_c_matrix(c_c, size)

    return result

# Función worker para realizar la multiplicación en un hilo
def worker(a, b, size):
    c = multiply_matrices(a, b, size)
    return c

def write_results_to_file(results, output_filename):
    with open(output_filename, "w") as file:
        for result in results:
            #file.write("Resultado de la multiplicación:\n")
            for row in result:
                file.write(' '.join(map(str, row)) + '\n')
            #file.write('\n')

# Modificar la función para usar ThreadPoolExecutor
def read_and_multiply_matrices(filename, num_threads, output_filename):
    global size, nump
    nump, size = read_matrices(filename)
    # Iniciar el cronómetro

    results = []

    start_time = time.time()

    with concurrent.futures.ThreadPoolExecutor(max_workers=num_threads) as executor:
        # Enviar las tareas al pool
        futures = [executor.submit(worker, a, b, size) for a, b in nump]

    # Detener el cronómetro
    end_time = time.time()

    # Recolectar los resultados en el orden de los futuros
    results = [future.result() for future in futures]

    # Escribir los resultados en el archivo en el orden correcto
    write_results_to_file(results, output_filename)

    # Calcular y mostrar la duración
    duration = end_time - start_time
    print(f"Tiempo de ejecución: {duration} segundos")

    with open('coarse_grain_python_graph.txt', 'a') as archivo:
        archivo.write(f'{num_threads} {duration}\n')

# Función para leer las matrices de un archivo
def read_matrices(filename):
    with open(filename, 'r') as file:
        global num_pairs
        num_pairs, size = map(int, file.readline().split())
        pairs = []
        for _ in range(num_pairs):
            a = [list(map(float, file.readline().split())) for _ in range(size)]
            b = [list(map(float, file.readline().split())) for _ in range(size)]
            pairs.append((a, b))
        return pairs, size

def read_matrix_from_file(filename, num_matrices):
    matrices_ = []
    with open(filename, 'r') as file:
        for _ in range(num_matrices):
            matrix = [list(map(float, file.readline().split())) for _ in range(size)]
            matrices_.append(matrix)
    return matrices_

def compare_matrices(a, b, epsilon=0.001):
    #size = len(a)
    for i in range(size):
        for j in range(size):
            if abs(a[i][j] - b[i][j]) > epsilon:
                return False
    return True

# Leer el número de hilos desde la consola
# if len(sys.argv) != 3:
#     print("Uso: python3 script.py <archivo_de_matrices> <numero_de_hilos>")
#     sys.exit(1)

filename = "matrices_large.dat"
num_threads = 1
max_threads = 32

# # Verificar que num_threads esté entre 1 y 32
# if not 1 <= num_threads <= 32:
#     print("Error: El número de hilos debe estar entre 1 y 32.")
#     sys.exit(1)

os.remove('coarse_grain_python_graph.txt')
os.remove('coarse_grain_python_graph_full.txt')
for num_threads in range(1,max_threads+1):
    start_time_full = time.time()
    # Ejemplo de uso
    read_and_multiply_matrices(filename, num_threads, "coarse_grain_version_python.txt")
    #read_and_multiply_matrices("matrices_dev.dat", num_threads, "resultados.txt")

    matrices1 = read_matrix_from_file("coarse_grain_version_python.txt", num_pairs)
    matrices2 = read_matrix_from_file("resultado_original.txt", num_pairs)

    for i, (mat1, mat2) in enumerate(zip(matrices1, matrices2)):
        if compare_matrices(mat1, mat2):
            # print(f"Las matrices {i+1} son iguales")
            continue
        else:
            print(f"Las matrices {i+1} son diferentes")
    end_time_full = time.time()
        # Calcular y mostrar la duración
    duration_full = end_time_full - start_time_full
    print(f"Tiempo de ejecución completo: {duration_full} segundos")

    with open('coarse_grain_python_graph_full.txt', 'a') as archivo:
        archivo.write(f'{num_threads} {duration_full}\n')