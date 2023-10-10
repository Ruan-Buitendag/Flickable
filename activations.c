//
// Created by ruanb on 9/10/2023.
//

#include "activations.h"
#include "math.h"
#include "stdio.h"
#include "wav.h"
#include "stft.h"

const char * H_persisted_dir = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano Transcription (C)\\data_persisted\\activations\\";

double BetaDivergence(const Matrix *x, const Matrix *y, double beta) {
    Matrix d = CreateMatrix(x->rows, x->cols);

    double sum = 0;

    for (int i = 0; i < x->rows; i++) {
        for (int j = 0; j < x->cols; j++) {
            if (y->array[i][j] < 1e-8) {
                y->array[i][j] = 1e-8;
            }
            if (x->array[i][j] < 1e-8) {
                x->array[i][j] = 1e-8;
            }

            d.array[i][j] = x->array[i][j] * log(x->array[i][j] / y->array[i][j]) - x->array[i][j] + y->array[i][j];


//            if (isnan(d.array[i][j])) {
//                printf("x: %f, y: %f\n", x->array[i][j], y->array[i][j]);
//            }

            sum += d.array[i][j];

        }
    }

    DestroyMatrix(&d);

    return sum;

}

double Gaussian(double x, double y){
    double exponent = -(pow(x, 2)/(2) + pow(y, 2)/(2));
    return exp(exponent);
}

Matrix ComputeActivations(const Spectrogram *X, unsigned int iterations, double beta, double maximum_error,
                          Dictionary *dictionary) {

    unsigned int r = dictionary->shape[2];
    unsigned int ncol = X->matrix.cols;
    unsigned int T = dictionary->shape[0];

    Matrix activations = CreateMatrix(r, ncol);

    for (int ii = 0; ii < activations.rows; ii++) {
        for (int jj = 0; jj < activations.cols; jj++) {
//            activations.array[ii][jj] = (double) rand() / (double) RAND_MAX;
            double x = (double)ii /activations.rows * 14 - 7;
//            double x = 0;
            double y = (double)jj/activations.cols * 14 - 7;
//            double y = 0;
            activations.array[ii][jj] = Gaussian(x, y);
        }
    }

    SaveMatrixToCSV("Initialised H.csv", &activations);

    if (beta != 1) {
        fprintf(stderr, "ComputeActivations: beta != 1 not implemented yet\n");
    }

    Matrix conved = ComputeConvolution(dictionary, &activations, T);


    double error_int = BetaDivergence(&X->matrix, &conved, beta);

    printf("ComputeActivations: Initial error = %f\n", error_int);

    Matrix convolutions[T];

    Matrix ones = CreateMatrix(dictionary->shape[1], ncol);
    FillMatrix(&ones, 1);

    for (int t = 0; t < T; t++) {
        Spectrogram tspec = GetSpectrogramFromDictionary(dictionary, 0, t);
        Matrix tspec_transposed = Transpose(&tspec.matrix);

        convolutions[t] = MatrixMultiply(&tspec_transposed, &ones);

        DestroySpectrogram(&tspec);
        DestroyMatrix(&tspec_transposed);
    }

    DestroyMatrix(&ones);

    Matrix denom_all_col = SumMatrices(convolutions, T);


//    SaveMatrixToCSV("denom_all_col.csv", &denom_all_col);


    for (int i = 0; i < T; i++) {
        DestroyMatrix(&convolutions[i]);
    }

    Matrix denoms_cropped_for_end = CreateMatrix(T, 88);


    for (int j = 1; j < T + 1; j++) {

        Matrix temp = CreateMatrix(T, 88);

        for (int a = 0; a < j; a++) {
            Spectrogram a_spec = GetSpectrogramFromDictionary(dictionary, 0, a);
            for (int cc = 0; cc < a_spec.matrix.cols; cc++) {
                for (int rr = 0; rr < a_spec.matrix.rows; rr++) {
                    temp.array[a][cc] += a_spec.matrix.array[rr][cc];
                }
            }

            DestroySpectrogram(&a_spec);

        }

        for (int cc = 0; cc < 88; cc++) {
            for (int rr = 0; rr < j; rr++) {
                denoms_cropped_for_end.array[j - 1][cc] += temp.array[rr][cc];
            }
        }

        DestroyMatrix(&temp);
    }

//    SaveMatrixToCSV("denoms_cropped_for_end.csv", &denoms_cropped_for_end);


    unsigned int iteration = 0;
    double obj, obj_prev = 0;

    while (iteration < iterations) {
        Matrix A = ComputeConvolution(dictionary, &activations, T);

//        SaveMatrixToCSV("A.csv", &A);

      // TODO: optimize the padding


//#pragma omp parallel for
        \
        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j < A.cols; j++) {

                if (A.array[i][j] < 1e-8) {
                    A.array[i][j] = 1e-8;
                }

                A.array[i][j] = X->matrix.array[i][j] * 1 / A.array[i][j];

            }
        }

//        SaveMatrixToCSV("X_hadamard_A.csv", &A);

        Matrix X_hadamard_A_padded = CreateMatrix(A.rows, A.cols + T);

//#pragma omp parallel for

        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j < A.cols; j++) {
                X_hadamard_A_padded.array[i][j] = A.array[i][j];
            }
        }

//        SaveMatrixToCSV("X_hadamard_A_padded.csv", &X_hadamard_A_padded);


        Matrix num = CreateMatrix(r, A.cols);

        DestroyMatrix(&A);

        for (int t = 0; t < T; t++) {
            Matrix W_at_t = GetMatrixFromDictionary(dictionary, 0, t);
            Matrix transposed = Transpose(&W_at_t);

            DestroyMatrix(&W_at_t);

            Matrix X_hadamard_A_windowed = CreateMatrix(X_hadamard_A_padded.rows, ncol);

            for (int i = 0; i < X_hadamard_A_padded.rows; i++) {
                for (int j = 0; j < ncol; j++) {
                    X_hadamard_A_windowed.array[i][j] = X_hadamard_A_padded.array[i][j + t];
                }
            }

            Matrix multiplied = MatrixMultiply(&transposed, &X_hadamard_A_windowed);

            DestroyMatrix(&X_hadamard_A_windowed);
            DestroyMatrix(&transposed);

            for (int i = 0; i < multiplied.rows; i++) {
                for (int j = 0; j < multiplied.cols; j++) {
                    num.array[i][j] += multiplied.array[i][j];
                }
            }
        }

        DestroyMatrix(&X_hadamard_A_padded);


//        SaveMatrixToCSV("num.csv", &num);

//#pragma omp parallel for

        for (int row = 0; row < activations.rows; row++) {
            for (int c = 0; c < ncol - T; c++) {
                if (denom_all_col.array[row][c] < 1e-8) {
                    denom_all_col.array[row][c] = 1e-8;
                }
                activations.array[row][c] *= (num.array[row][c] / denom_all_col.array[row][c]);
            }
        }

//        SaveMatrixToCSV("H1.csv", &activations);
//#pragma omp parallel for

        for (int c = (int) (ncol - T); c < ncol; c++) {
            for (int row = 0; row < activations.rows; row++) {
                activations.array[row][c] *=
                        (num.array[row][c] / denoms_cropped_for_end.array[ncol - c - 1][row]);
            }
        }

//        SaveMatrixToCSV("H2.csv", &activations);

        DestroyMatrix(&num);
        DestroyMatrix(&conved);

        conved = ComputeConvolution(dictionary, &activations, T);

        obj = BetaDivergence(&X->matrix, &conved, beta);

        printf("ComputeActivations: Iteration %d, obj = %f\n", iteration, obj);

        if ((fabs(obj - obj_prev) / error_int) < maximum_error) {
            printf("ComputeActivations: Converged sufficiently\n");
//            break;
        }

        obj_prev = obj;
        iteration++;
    }

    DestroyMatrix(&denom_all_col);
    DestroyMatrix(&denoms_cropped_for_end);

    return activations;
}

Matrix ComputeConvolution(const Dictionary *dictionary, const Matrix *matrix2, unsigned int t) {
    Matrix convolutions[t];

    for (int i = 0; i < t; i++) {
        Matrix tspec = GetMatrixFromDictionary(dictionary, 0, i);

        Matrix shifted = ShiftMatrix(matrix2, i);
        convolutions[i] = MatrixMultiply(&tspec, &shifted);

        DestroyMatrix(&tspec);
        DestroyMatrix(&shifted);
    }

    Matrix conv_sum = SumMatricesAlongAxis(convolutions, t, 0);

    for (int i = 0; i < t; i++) {
        DestroyMatrix(&convolutions[i]);
    }

    return conv_sum;
}


void TestActivations() {

}

Matrix GetActivationsFromFile(const char *filename, Dictionary *dictionary, double time_limit, int iterations) {
    WavFile wav = ReadWav(filename);

    DynamicArray mono = StereoToMono(&wav, "average");

    Spectrogram spec = STFT(&mono, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

//    Dictionary aaaa = HardFilterSpectrograms(dictionary, 1500);

//    NormaliseDictionary(&aaaa);

    DestroySpectrogram(&spec);

    Matrix activations = ComputeActivations(&filtered, iterations, 1, 0.1, dictionary);

    char H_persisted_name[1000];

    snprintf(H_persisted_name, sizeof(H_persisted_name), "%s%s_H_%d_iterations_limited_to_%lfs_.csv", H_persisted_dir, filename, iterations, time_limit);

    SaveMatrixToCSV(H_persisted_name, &activations);

    return activations;
}



Matrix GetActivations(const char *filename, Dictionary* dictionary, double time_limit, int iterations, bool recompute) {
//    Dictionary dict = GetDictionary("AkPnBcht");

//    Dictionary dict = GetDictionary("AkPnBsdf");


    char H_persisted_name[1000];

    snprintf(H_persisted_name, sizeof(H_persisted_name), "%s%s_H_%d_iterations_limited_to_%lfs_.csv", H_persisted_dir, filename, iterations, time_limit);

    Matrix activations = LoadMatrixFromCSV(H_persisted_name);

    if ((activations.rows == 0 && activations.cols == 0) || recompute) {
        activations = GetActivationsFromFile(filename, dictionary, time_limit, iterations);
    }

    return activations;
}


Matrix GetActivationsFromArray(DynamicArray *array, Dictionary *dictionary, double time_limit, int iterations)
{
    Spectrogram spec = STFT(array, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

//    Dictionary aaaa = HardFilterSpectrograms(dictionary, 1500);

//    NormaliseDictionary(&aaaa);

    DestroySpectrogram(&spec);

    Matrix activations = ComputeActivations(&filtered, iterations, 1, 0.1, dictionary);

    return activations;
}
