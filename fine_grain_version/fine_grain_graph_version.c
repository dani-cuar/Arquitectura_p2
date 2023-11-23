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
    int startRow;
    int endRow;
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
    Task *task = (Task *) arg;
    //printf("Hilo iniciado para multiplicar matrices.\n");
    int i, j, k;
    double sum;

    for (i = task->startRow; i < task->endRow; i++) {
        for (j = 0; j < matrixSize; j++) {
            sum = 0.0;
            for (k = 0; k < matrixSize; k++) {
                sum += task->matrices->a[i][k] * task->matrices->b[k][j];    
            }
            task->matrices->c[i][j] = sum;
        }
    }
    //printf("Hilo completado para multiplicar matrices.\n");
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

    int numThreads;
    int maxThreads = 32;

    // // Pedir al usuario que ingrese el numero de hilos
    // printf("Ingrese el número de hilos (1-32): ");
    // scanf("%d", &numThreads);

    // // Validar la entrada
    // if (numThreads < 1 || numThreads > 32) {
    //     printf("Número de hilos fuera del rango permitido. Usando valor predeterminado de 4.\n");
    //     numThreads = 4;
    // }

    remove("fine_grain_graph.txt");
    remove("fine_grain_graph_full.txt");
    for (numThreads = 1; numThreads <= maxThreads; numThreads++)
    {
        struct timespec start_full, end_full;  // Variables para almacenar los tiempos de todo el programa
        clock_gettime(CLOCK_MONOTONIC, &start_full);  // Iniciar cronómetro

        FILE *fh;
        char *fname = "matrices_large.dat";

        printf("Start\n");
        fh = fopen(fname, "r");
        if (fh == NULL) {
            perror("Error al abrir el archivo");
            return -1;
        }

        fscanf(fh, "%d %d\n", &nmats, &matrixSize);

        pthread_mutex_t taskMutex;
        pthread_mutex_init(&taskMutex, NULL);

        double totalTime = 0.0; // Variable para almacenar el tiempo total
        struct timespec start, end; // Variables para los tiempos de inicio y fin

        for (int pair = 0; pair < nmats; pair++) {
            double **a = allocateMatrix(matrixSize);
            double **b = allocateMatrix(matrixSize);

            //printf("Matriz A:\n");
            for (int i = 0; i < matrixSize; i++) {
                for (int j = 0; j < matrixSize; j++) {
                    fscanf(fh, "%lf", &a[i][j]);
                    //printf("%lf ", a[i][j]);
                }
                //printf("\n");
            }

            //printf("Matriz B:\n");
            for (int i = 0; i < matrixSize; i++) {
                for (int j = 0; j < matrixSize; j++) {
                    fscanf(fh, "%lf", &b[i][j]);
                    //printf("%lf ", b[i][j]);
                }
                //printf("\n");
            }

            Task *taskQueue = malloc(numThreads * sizeof(Task));
            int rowsPerThread = matrixSize / numThreads;
            int extraRows = matrixSize % numThreads;
            int currentRow = 0;

            for (int i = 0; i < numThreads; i++) {
                taskQueue[i].matrices = malloc(sizeof(Matrices));
                taskQueue[i].matrices->a = a;
                taskQueue[i].matrices->b = b;
                taskQueue[i].matrices->c = allocateMatrix(matrixSize);

                // Inicializar la matriz C a cero
                for (int j = 0; j < matrixSize; j++) {
                    for (int k = 0; k < matrixSize; k++) {
                        taskQueue[i].matrices->c[j][k] = 0.0;
                    }
                }
                taskQueue[i].startRow = currentRow;
                currentRow += rowsPerThread + (i < extraRows ? 1 : 0);
                taskQueue[i].endRow = currentRow;
            }

            // Iniciar el temporizador antes de crear los hilos
            clock_gettime(CLOCK_MONOTONIC, &start);
            pthread_t threads[numThreads];
            for (int i = 0; i < numThreads; i++) {
                pthread_create(&threads[i], NULL, mm, &taskQueue[i]);
            }

            for (int i = 0; i < numThreads; i++) {
                pthread_join(threads[i], NULL);
            }

            // Detener el temporizador y calcular el tiempo para este par
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken = end.tv_sec - start.tv_sec;
            time_taken += (end.tv_nsec - start.tv_nsec) / 1e9;
            totalTime += time_taken; // Acumular el tiempo

            // Abrir el archivo para agregar los resultados de este par de matrices
            FILE *outputFile = fopen("fine_grain_version_result.txt", "a");
            if (outputFile == NULL) {
                perror("Error al abrir el archivo de salida");
                return -1;
            }

            // Escribir el resultado de la multiplicación de este par en el archivo
            //printf("Resultado de la multiplicación del par %d:\n", pair + 1);
            for (int i = 0; i < matrixSize; i++) {
                for (int j = 0; j < matrixSize; j++) {
                    double sum = 0.0;
                    for (int k = 0; k < numThreads; k++) {
                        sum += taskQueue[k].matrices->c[i][j];
                    }
                    fprintf(outputFile, "%lf ", sum);
                    //printf("%lf ", sum);
                }
                fprintf(outputFile, "\n");
                //printf("\n");
            }

            fclose(outputFile);

            // Liberar memoria
            for (int i = 0; i < numThreads; i++) {
                free(taskQueue[i].matrices->c[0]);
                free(taskQueue[i].matrices->c);
                free(taskQueue[i].matrices);
            }
            free(taskQueue);
            free(a[0]);
            free(a);
            free(b[0]);
            free(b);
        }

        // Guardar archivo para graficar
        FILE *file = fopen("fine_grain_graph.txt", "a");
        if (file == NULL) {
            perror("Error al abrir el archivo de grafica");
            exit(1);
        }
        fprintf(file, "%d %lf\n", numThreads, totalTime);
        fclose(file);

        // Imprimir el tiempo total después de procesar todos los pares
        printf("Tiempo total de ejecución: %f segundos\n", totalTime);
        pthread_mutex_destroy(&taskMutex);

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
        if (!read_matrices("fine_grain_version_result.txt", matrices1, nmats, matrixSize) || 
            !read_matrices("resultado_original.txt", matrices2, nmats, matrixSize)) {
            printf("Error al leer los archivos\n");
            return 1;
        }

        // Comparar las matrices
        for (int k = 0; k < nmats; k++) {
            if (compare_matrices(matrices1[k], matrices2[k], matrixSize)) {
                // printf("Las matrices %d son iguales\n", k+1);
                continue;
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

        clock_gettime(CLOCK_MONOTONIC, &end_full);  // Detener cronómetro de todo el codigo

        // Calcular y mostrar el tiempo de ejecución completo
        double time_taken_full = end_full.tv_sec - start_full.tv_sec;
        time_taken_full += (end_full.tv_nsec - start_full.tv_nsec) / 1e9;  // Convertir nanosegundos a segundos
        printf("Tiempo de ejecución completo: %f segundos\n", time_taken_full);

        // Guardar archivo para graficar
        FILE *file2 = fopen("fine_grain_graph_full.txt", "a");
        if (file2 == NULL) {
            perror("Error al abrir el archivo de grafica");
            exit(1);
        }
        fprintf(file2, "%d %lf\n", numThreads, time_taken_full);
        fclose(file2);
    }
    return 0;
}
