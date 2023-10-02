//
// Created by ruanb on 9/6/2023.
//

#include <stdbool.h>
#include "spectrogram.h"
#include "stdlib.h"
#include "stdio.h"

Spectrogram CreateSpectrogram(unsigned int nRows, unsigned int nCols) {
    Spectrogram spectrogram;
    spectrogram.matrix = CreateMatrix(nRows, nCols);

    return spectrogram;
}

Spectrogram HardFilterSpectrogram(Spectrogram const * spectrogram, unsigned int numNewRows) {
    Spectrogram filtered = CreateSpectrogram(numNewRows, spectrogram->matrix.cols);

    filtered.timeStep = spectrogram->timeStep;
    filtered.frequencyStep = spectrogram->frequencyStep;

    for(int r = 0; r < numNewRows; r++){
        for(int c = 0; c < spectrogram->matrix.cols; c++){
            filtered.matrix.array[r][c] = spectrogram->matrix.array[r][c];
        }
    }

    return filtered;
}

void SaveSpectrogramToCSV(const char *filename, Spectrogram* spectrogram) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "SaveSpectrogramToCSV: Error opening file");
        return;
    }

    // Write the array data to the CSV file
    for (int i = 0; i < spectrogram->matrix.rows; i++) {
        for (int j = 0; j < spectrogram->matrix.cols; j++) {
            fprintf(file, "%.6f", spectrogram->matrix.array[i][j]); // Adjust the format specifier as needed
            if (j < spectrogram->matrix.cols - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}


void DestroySpectrogram(Spectrogram *spectrogram) {
    DestroyMatrix(&spectrogram->matrix);
}

void NormalizeSpectrogram(Spectrogram *spectrogram) {
    double max = 0;

    for(int r = 0; r < spectrogram->matrix.rows; r++){
        for(int c = 0; c < spectrogram->matrix.cols; c++){
            if(spectrogram->matrix.array[r][c] > max){
                max = spectrogram->matrix.array[r][c];
            }
        }
    }

    for(int r = 0; r < spectrogram->matrix.rows; r++){
        for(int c = 0; c < spectrogram->matrix.cols; c++){
            spectrogram->matrix.array[r][c] /= max;
        }
    }
}

double GetDelay(const Spectrogram *spectrogram, double threshold) {
    int column = 0;

    bool foundStart = false;

    while (!foundStart) {
        for (int i = 0; i < spectrogram->matrix.rows; i++) {
            if (spectrogram->matrix.array[i][column] > threshold) {
                foundStart = true;
                break;
            }
        }
        column++;
    }

    double delay = --column * (spectrogram->timeStep);

    return delay;
}

