#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double **a;
    double **b;
    double **c;
    int size;
} Matrices;

double **allocate_matrix(int size) {
    double **matrix = malloc(size * sizeof(double *));
    if (matrix == NULL) {
        perror("Error al asignar memoria para la matriz");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        matrix[i] = malloc(size * sizeof(double));
        if (matrix[i] == NULL) {
            perror("Error al asignar memoria para la fila de la matriz");
            exit(1);
        }
    }
    return matrix;
}

void free_matrix(double **matrix, int size) {
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Función para multiplicar matrices
void mm(Matrices *mats) {  
    for (int i = 0; i < mats->size; i++) {
        for (int j = 0; j < mats->size; j++) {
            double sum = 0.0;
            for (int k = 0; k < mats->size; k++) {
                sum += mats->a[i][k] * mats->b[k][j];
            }
            mats->c[i][j] = sum;
        }
    }
}

// Función para exportar que envuelve 'mm'
void multiply_matrices(double **a, double **b, double **c, int size) {
    Matrices mats;
    mats.a = a;
    mats.b = b;
    mats.c = c;
    mats.size = size;
    mm(&mats);
}
