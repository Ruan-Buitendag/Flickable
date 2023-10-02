//
// Created by ruanb on 9/12/2023.
//

#include <string.h>
#include "matrix.h"
#include "stdlib.h"
#include "stdio.h"


Matrix CreateMatrix(unsigned int nRows, unsigned int nCols) {
    Matrix matrix;
    matrix.array = (double **) malloc(nRows * sizeof(double *));

    for (int i = 0; i < nRows; i++) {
        matrix.array[i] = (double *) calloc(nCols, sizeof(double));
    }

    matrix.rows = nRows;
    matrix.cols = nCols;

    return matrix;
}

void DestroyMatrix(Matrix *matrix) {
    if (matrix->array == NULL) {
        return;
    }

    for (int i = 0; i < matrix->rows; i++) {
        free(matrix->array[i]);
    }

    free(matrix->array);
    matrix->array = NULL;
}

Matrix MatrixMultiply(const Matrix *a, const Matrix *b) {
    Matrix result = CreateMatrix(a->rows, b->cols);

    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            for (int k = 0; k < a->cols; k++) {
                result.array[i][j] += a->array[i][k] * b->array[k][j];
            }
        }
    }

    return result;
}

Matrix SumMatrices(Matrix *a, unsigned int numMatrices) {
    Matrix result = CreateMatrix(a[0].rows, a[0].cols);

    for (int i = 0; i < numMatrices; i++) {
        for (int j = 0; j < a[i].rows; j++) {
            for (int k = 0; k < a[i].cols; k++) {
                result.array[j][k] += a[i].array[j][k];
            }
        }
    }

    return result;

}

Matrix SumMatricesAlongAxis(Matrix *a, unsigned int numMatrices, unsigned int axis) {
    // TODO: optimisation is probably possible here
    Matrix result = CreateMatrix(a[0].rows, a[0].cols);

    for (int r = 0; r < result.rows; r++) {
        for (int c = 0; c < result.cols; c++) {
            for (int i = 0; i < numMatrices; i++) {
                if (axis == 0) {
                    result.array[r][c] += a[i].array[r][c];
                } else if (axis == 1) {
                    result.array[r][c] += a[i].array[c][r];
                }
            }
        }
    }

    return result;

}

Matrix Transpose(const Matrix *matrix) {
    Matrix transposed = CreateMatrix(matrix->cols, matrix->rows);

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            transposed.array[j][i] = matrix->array[i][j];
        }
    }

    return transposed;
}

void FillMatrix(Matrix *matrix, double value) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            matrix->array[i][j] = value;
        }
    }
}

void NormaliseMatrix(Matrix *matrix) {
    double max = 0;
    for (int r = 0; r < matrix->rows; r++) {
        for (int c = 0; c < matrix->cols; c++) {
            if (matrix->array[r][c] > max)
                max = matrix->array[r][c];
        }
    }

    for (int r = 0; r < matrix->rows; r++) {
        for (int c = 0; c < matrix->cols; c++) {
            matrix->array[r][c] /= max;
        }
    }
}

Matrix ShiftMatrix(const Matrix *matrix, unsigned int numShifts) {
    Matrix shifted = CreateMatrix(matrix->rows, matrix->cols);

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            if (j + numShifts < matrix->cols) {
                shifted.array[i][j + numShifts] = matrix->array[i][j];
            }
        }
    }

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < numShifts; j++) {
            shifted.array[i][j] = 0;

        }
    }

    return shifted;
}

void SaveMatrixToCSV(const char *filename, Matrix const *matrix) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "SaveMatrixToCSV: Error opening file");
        return;
    }

    // Write the array data to the CSV file
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            fprintf(file, "%.6f", matrix->array[i][j]); // Adjust the format specifier as needed
            if (j < matrix->cols - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

Matrix LoadMatrixFromCSV(const char *filename) {
//    determine how many rows in the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "LoadMatrixFromCSV: Error opening file");
        return CreateMatrix(0, 0);
//        exit(1);
    }

    int rows = 0;
    int cols = 1;

    while (!feof(file)) {
        char ch = fgetc(file);
        if (ch == '\n') {
            rows++;
        } else if (ch == ',' && rows == 0) {
            cols++;
        }
    }

    Matrix result = CreateMatrix(rows, cols);

//    set the file pointer back to the start of the file
    fseek(file, 0, SEEK_SET);

//    read the data into the matrix
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols - 1; c++) {
            fscanf(file, "%lf,", &result.array[r][c]);
        }
        fscanf(file, "%lf\n", &result.array[r][cols - 1]);
    }

    return result;
}

void matrixTest() {

    Matrix ass = CreateMatrix(10, 10);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            ass.array[i][j] = i + j;
        }
    }

    SaveMatrixToCSV("matrixloadtest.csv", &ass);


    Matrix assjole = LoadMatrixFromCSV("matrixloadtest.csv");

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%lf  ", assjole.array[i][j]);
        }
    }
}




