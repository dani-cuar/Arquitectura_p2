#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 100  // Tamaño de la matriz
#define NUM_MATRICES 50  // Número de matrices en cada archivo
#define EPSILON 0.001  // Umbral para comparación de flotantes

// Función para comparar dos matrices
int compare_matrices(float a[N][N], float b[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (fabs(a[i][j] - b[i][j]) > EPSILON) {
                return 0;  // Las matrices son diferentes
            }
        }
    }
    return 1;  // Las matrices son iguales
}

// Función para leer matrices de un archivo
int read_matrices(const char *filename, float matrices[NUM_MATRICES][N][N]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 0;
    }

    for (int k = 0; k < NUM_MATRICES; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (fscanf(file, "%f", &matrices[k][i][j]) != 1) {
                    fclose(file);
                    return 0;  // Error en la lectura
                }
            }
        }
    }

    fclose(file);
    return 1;
}

int main() {
    float matrices1[NUM_MATRICES][N][N], matrices2[NUM_MATRICES][N][N];

    if (!read_matrices("multiplicacion_threads.txt", matrices1) || !read_matrices("resultado_original.txt", matrices2)) {
        printf("Error al leer los archivos\n");
        return 1;
    }

    for (int k = 0; k < NUM_MATRICES; k++) {
        if (compare_matrices(matrices1[k], matrices2[k])) {
            printf("Las matrices %d son iguales\n", k + 1);
        } else {
            printf("Las matrices %d son diferentes\n", k + 1);
        }
    }

    return 0;
}
