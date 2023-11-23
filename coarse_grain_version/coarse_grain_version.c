#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include <math.h>

#define EPSILON 0.001  // Umbral para comparación de flotantes
int matrixSize;
int numThreads;
int nmats;
pthread_mutex_t taskMutex;

int currentTask = 0;

//----- sección de funciones para comparar resultados------------

// Función para comparar dos matrices
int compare_matrices(double **a, double **b, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (fabs(a[i][j] - b[i][j]) > EPSILON) {
                return 0;  // Las matrices son diferentes
            }
        }
    }
    return 1;  // Las matrices son iguales
}

// Función para leer matrices de un archivo
int read_matrices(const char *filename, double ***matrices, int nmats, int size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 0;
    }

    for (int k = 0; k < nmats; k++) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (fscanf(file, "%lf", &matrices[k][i][j]) != 1) {
                    fclose(file);
                    return 0;  // Error en la lectura
                }
            }
        }
    }

    fclose(file);
    return 1;
}

//--------------------------------------------------------------------
//--------------- Sección de codificación multithread ----------------
typedef struct {
    double **a;
    double **b;
    double **c;
} Matrices;

// Estructura de tarea
typedef struct {
    Matrices *matrices; // Apunta a las matrices a multiplicar
    int taskCompleted; // Indica si la tarea ha sido completada
} Task;

Task *taskQueue;

double **allocateMatrix() {
    int i;
    double *vals, **temp;

    vals = (double *) malloc(matrixSize * matrixSize * sizeof(double));
    temp = (double **) malloc(matrixSize * sizeof(double*));

    for (i = 0; i < matrixSize; i++)
        temp[i] = &(vals[i * matrixSize]);

    return temp;
}

void *mm(void *arg) {
    Matrices *mats = (Matrices *) arg;
    //printf("Hilo iniciado para multiplicar matrices.\n");
    int i, j, k;
    double sum;

    for (i = 0; i < matrixSize; i++) {
        for (j = 0; j < matrixSize; j++) {
            sum = 0.0;
            for (k = 0; k < matrixSize; k++) {
                sum += mats->a[i][k] * mats->b[k][j];
            }
            mats->c[i][j] = sum;
        }
    }
    //printf("Hilo completado para multiplicar matrices.\n");
    return NULL;
}

void *threadFunction(void *arg) {
    while (1) {
        pthread_mutex_lock(&taskMutex);
        if (currentTask >= nmats) {
            pthread_mutex_unlock(&taskMutex);
            break;
        }

        Task task = taskQueue[currentTask];
        currentTask++;
        //printf("Tarea actual: %d",currentTask);
        pthread_mutex_unlock(&taskMutex);

        if (!task.taskCompleted) {
            mm(task.matrices);
            task.taskCompleted = 1;
            //printAndSaveResult(task.matrices->c, "resultado_hilo.txt");
        }
    }
    return NULL;
}

void printMatrixToFile(double **matrix, int size, FILE *file) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fprintf(file, "%lf ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    //fprintf(file, "\n"); // Separar las matrices por una línea vacía
}

int main(void) {
    FILE *fh;
    char *fname = "matrices_large.dat";
    int numThreads;

    // Pedir al usuario que ingrese el numero de hilos
    printf("Ingrese el número de hilos (1-32): ");
    scanf("%d", &numThreads);

    // Validar la entrada
    if (numThreads < 1 || numThreads > 32) {
        printf("Número de hilos fuera del rango permitido. Usando valor predeterminado de 4.\n");
        numThreads = 4;
    }

    printf("Start\n");
    fh = fopen(fname, "r");
    if (fh == NULL) {
        perror("Error al abrir el archivo");
        return -1;
    }

    fscanf(fh, "%d %d\n", &nmats, &matrixSize);

    // Asignar memoria para taskQueue basado en nmats
    taskQueue = malloc(nmats * sizeof(Task));
    if (taskQueue == NULL) {
        perror("Error al asignar memoria para taskQueue");
        return -1;
    }

    for (int i = 0; i < nmats; i++) {
        taskQueue[i].matrices = malloc(sizeof(Matrices));
        taskQueue[i].matrices->a = allocateMatrix();
        taskQueue[i].matrices->b = allocateMatrix();
        taskQueue[i].matrices->c = allocateMatrix();
        taskQueue[i].taskCompleted = 0;

        // Leer y asignar valores a las matrices A y B
        for (int j = 0; j < matrixSize; j++) {
            for (int k = 0; k < matrixSize; k++) {
                fscanf(fh, "%lf", &taskQueue[i].matrices->a[j][k]);
            }
        }
        for (int j = 0; j < matrixSize; j++) {
            for (int k = 0; k < matrixSize; k++) {
                fscanf(fh, "%lf", &taskQueue[i].matrices->b[j][k]);
            }
        }
    }

    fclose(fh);

    /*for (int i = 0; i < nmats; i++) {
        fprintf(outputFile, "Tarea %d - Matriz A:\n", i);
        printMatrixToFile(taskQueue[i].matrices->a, matrixSize, outputFile);

        fprintf(outputFile, "Tarea %d - Matriz B:\n", i);
        printMatrixToFile(taskQueue[i].matrices->b, matrixSize, outputFile);
    }*/

    struct timespec start, end;  // Variables para almacenar los tiempos
    clock_gettime(CLOCK_MONOTONIC, &start);  // Iniciar cronómetro

    pthread_t threads[numThreads];
    //printf("Creando %d hilos \n", numThreads);
    pthread_mutex_init(&taskMutex, NULL);
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, threadFunction, NULL);
        //printf("Creando hilo %d \n", i);
    }

    // Esperar a que los hilos terminen
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);  // Detener cronómetro

    // Calcular y mostrar el tiempo de ejecución
    double time_taken = end.tv_sec - start.tv_sec;
    time_taken += (end.tv_nsec - start.tv_nsec) / 1e9;  // Convertir nanosegundos a segundos
    printf("Tiempo de ejecución: %f segundos\n", time_taken);

    FILE *outputFile = fopen("coarse_grain_version_result.txt", "w");
    if (outputFile == NULL) {
        perror("Error al abrir el archivo de salida");
        return -1;
    }
    for (int i = 0; i < nmats; i++) {
        //fprintf(outputFile, "Tarea %d - Matriz C:\n", i);
        printMatrixToFile(taskQueue[i].matrices->c, matrixSize, outputFile);
    }

    fclose(outputFile);

    // Liberar memoria de las tareas y matrices
    for (int i = 0; i < nmats; i++) {
        free(taskQueue[i].matrices->a);
        free(taskQueue[i].matrices->b);
        free(taskQueue[i].matrices->c);
        free(taskQueue[i].matrices);
    }

    pthread_mutex_destroy(&taskMutex);
    free(taskQueue);

    printf("Done.\n");

    //--------comparación de matrices ----------------------
    
    // Asignar memoria para las matrices dinámicas
    double ***matrices1 = malloc(nmats * sizeof(double**));
    double ***matrices2 = malloc(nmats * sizeof(double**));
    for (int k = 0; k < nmats; k++) {
        matrices1[k] = allocateMatrix();
        matrices2[k] = allocateMatrix();
    }

    // Leer las matrices de los archivos
    if (!read_matrices("coarse_grain_version_result.txt", matrices1, nmats, matrixSize) || 
        !read_matrices("resultado_original.txt", matrices2, nmats, matrixSize)) {
        printf("Error al leer los archivos\n");
        return 1;
    }

    // Comparar las matrices
    for (int k = 0; k < nmats; k++) {
        if (compare_matrices(matrices1[k], matrices2[k], matrixSize)) {
            printf("Las matrices %d son iguales\n", k+1);
        } else {
            printf("Las matrices %d son diferentes\n", k+1);
        }
    }

    // Liberar memoria de las matrices dinámicas
    for (int k = 0; k < nmats; k++) {
        free(matrices1[k][0]);  // Liberar el bloque de memoria de valores
        free(matrices1[k]);     // Liberar los punteros
        free(matrices2[k][0]);
        free(matrices2[k]);
    }
    free(matrices1);
    free(matrices2);

    return 0;
}
