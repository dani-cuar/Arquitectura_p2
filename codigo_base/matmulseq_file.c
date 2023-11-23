//Matrix multiplication

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

double **a, **b, **c;
int matrixSize;

double **allocateMatrix() {
	int i;
	double *vals, **temp;

	// allocate space for values of a matrix
	vals = (double *) malloc (matrixSize * matrixSize * sizeof(double));

	// allocate vector of pointers to create the 2D array
	temp = (double **) malloc (matrixSize * sizeof(double*));

	for(i=0; i < matrixSize; i++)
		temp[i] = &(vals[i * matrixSize]);

	return temp;
}

void mm(void) {
	int i,j,k;
	double sum;
	// matrix multiplication
	for (i = 0; i < matrixSize; i++) {
		for (j = 0; j < matrixSize; j++) {
			sum = 0.0;
			// dot product
			for (k = 0; k < matrixSize; k++) {
				sum = sum + a[i][k] * b[k][j];
			}
			c[i][j] = sum;
		}
	}
}
void printResult(void){
	int i, j;	
	for(i=0;i<matrixSize;i++){
		for(j=0;j<matrixSize;j++){
			printf("%lf ", c[i][j]);
		}
		printf("\n");
	}
}

void printAndSaveResult(const char* filename) {
    int i, j;
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }

    for (i = 0; i < matrixSize; i++) {
        for (j = 0; j < matrixSize; j++) {
            //printf("%lf ", c[i][j]);
            fprintf(file, "%lf ", c[i][j]);
        }
        //printf("\n");
        fprintf(file, "\n");
    }

    fclose(file);
}

int main(void) {
	struct timespec start_full, end_full;  // Variables para almacenar los tiempos
	clock_gettime(CLOCK_MONOTONIC, &start_full);  // Iniciar cronómetro
	int i, j, k;
	int nmats;
	char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
	FILE *fh;

	printf("Start\n");
	fh = fopen(fname, "r");
	//First line indicates how many pairs of matrices there are and the matrix size
	fscanf(fh, "%d %d\n", &nmats, &matrixSize);

	//Dynamically create matrices of the size needed
	a = allocateMatrix();
	b = allocateMatrix();
	c = allocateMatrix();

	struct timespec start, end;  // Variables para almacenar los tiempos
    clock_gettime(CLOCK_MONOTONIC, &start);  // Iniciar cronómetro

	//printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
	for(k=0;k<nmats;k++){
		for(i=0;i<matrixSize;i++){
			for(j=0;j<matrixSize;j++){
				fscanf(fh, "%lf", &a[i][j]);
			}
		}
		for(i=0;i<matrixSize;i++){
			for(j=0;j<matrixSize;j++){
				fscanf(fh, "%lf", &b[i][j]);
			}
		}

		//printf("Multiplying two matrices...\n"); //Remove this line for performance tests
		mm();
		//printResult(); //Remove this line for performance tests
		printAndSaveResult("resultado_original.txt");
	}

	clock_gettime(CLOCK_MONOTONIC, &end);  // Detener cronómetro

    // Calcular y mostrar el tiempo de ejecución
    double time_taken = end.tv_sec - start.tv_sec;
    time_taken += (end.tv_nsec - start.tv_nsec) / 1e9;  // Convertir nanosegundos a segundos
    printf("Tiempo de ejecución: %f segundos\n", time_taken);

	fclose(fh);

	// Free memory
	free(*a);
	free(a);
	free(*b);
	free(b);
	free(*c);
	free(c);
	printf("Done.\n");
	
	clock_gettime(CLOCK_MONOTONIC, &end_full);  // Detener cronómetro

    // Calcular y mostrar el tiempo de ejecución
    double time_taken_full = end_full.tv_sec - start_full.tv_sec;
    time_taken_full += (end_full.tv_nsec - start_full.tv_nsec) / 1e9;  // Convertir nanosegundos a segundos
    printf("Tiempo de ejecución completo: %f segundos\n", time_taken_full);

	return 0;

}

