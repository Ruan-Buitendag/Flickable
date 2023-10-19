//
// Created by ruanb on 9/12/2023.
//

#include <string.h>
#include "matrix.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

#include "omp.h"

#include "stdlib.h"
#include "windows.h"

#include "time.h"

//#include "cblas.h"

//#include "mkl.h"
//#include "mkl_cblas.h"

size_t cache_line_size() {
    size_t line_size = 0;
    DWORD buffer_size = 0;
    DWORD i = 0;
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

    GetLogicalProcessorInformation(0, &buffer_size);
    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
    GetLogicalProcessorInformation(&buffer[0], &buffer_size);

    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
            line_size = buffer[i].Cache.LineSize;
            break;
        }
    }

    free(buffer);
    return line_size;
}



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

//Matrix MatrixMultiply(const Matrix *a, const Matrix *b) {
//    Matrix result = CreateMatrix(a->rows, b->cols);

//    for (int i = 0; i < a->rows; i++) {
//        for (int j = 0; j < b->cols; j++) {
//            for (int k = 0; k < a->cols; k++) {
//                result.array[i][j] += a->array[i][k] * b->array[k][j];
//            }
//        }
//    }

//    return result;
//}

Matrix MatrixMultiply(const Matrix *a, const Matrix *b)
{
    int block_size = 32;

    int rows = a->rows;
    int cols = b->cols;
    int common_dim = a->cols;

    Matrix result = CreateMatrix(rows, cols);


//    double* result_array = calloc(result.cols * result.rows, sizeof(double));
//    double* a_array = calloc(a->cols * a->rows, sizeof(double));
//    double* b_array = calloc(b->cols * b->rows, sizeof(double));


//    for(int i = 0; i < a->rows; i++){
//        for(int j = 0; j < a->cols; j++){
//            a_array[i * a->cols + j] = a->array[i][j];
//        }
//    }

//    for(int i = 0; i < b->rows; i++){
//        for(int j = 0; j < b->cols; j++){
//            b_array[i * b->cols + j] = b->array[i][j];
//        }
//    }

//    const CBLAS_LAYOUT layout = CblasRowMajor; // or CblasColMajor for column-major
//    const CBLAS_TRANSPOSE TransA = CblasNoTrans; // or CblasTrans or CblasConjTrans
//    const CBLAS_TRANSPOSE TransB = CblasNoTrans; // or CblasTrans or CblasConjTrans
//    const double alpha = 1.0; // Scaling factor for A*B
//    const double beta = 0.0; // Scaling factor for C


//    cblas_dgemm(layout, TransA, TransB, a->rows, b->cols, a->cols, alpha, a_array, a->cols, b_array, b->cols, beta, result_array, a->rows);


    clock_t begin = clock();

#pragma omp parallel for collapse(2) schedule(dynamic, 1) shared(result, a, b)
    for (int i = 0; i < rows; i += block_size) {
        for (int k = 0; k < common_dim; k += block_size) {
            for (int j = 0; j < cols; j += block_size) {
                for (int ii = i; ii < i + block_size && ii < rows; ii++) {
                    for (int kk = k; kk < k + block_size && kk < common_dim; kk++) {
                        for (int jj = j; jj < j + block_size && jj < cols; jj++) {

                                result.array[ii][jj] += a->array[ii][kk] * b->array[kk][jj];

                        }
                    }
                }
            }
        }
  }

//    free(a_array);
//    free(b_array);

//    for(int i = 0; i < result.rows; i++){
//        for(int j = 0; j < result.cols; j++){
//            result.array[i][j] = result_array[i + j * result.cols];
//        }
//    }

//    free(result_array);


    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

//    printf("Time spent on matrix multiplication using matrices %lf", time_spent);

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
            fprintf(file, "%.10f", matrix->array[i][j]); // Adjust the format specifier as needed
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




double *FlattenMatrix(const Matrix *matrix)
{
    double* flattened = (double*) calloc(matrix->rows * matrix->rows, sizeof(double));

    for(int row = 0; row < matrix->rows; row++){
        for(int col = 0; col < matrix->cols; col++){
            flattened[row * matrix->cols + col] = matrix->array[row][col];
        }
    }


    return flattened;
}

Matrix ReshapeMatrix(double *array, unsigned int rows, unsigned int cols)
{
    Matrix matrix = CreateMatrix(rows, cols);

    for(int element = 0; element < matrix.rows * matrix.cols; element++){
        matrix.array[element / matrix.cols][element % matrix.cols] = array[element];
    }

    return matrix;
}
