//
// Created by ruanb on 9/6/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_SPECTROGRAM_H
#define C_PIANO_TRANSCRIPTION_SPECTROGRAM_H

#include "matrix.h"

typedef struct SpectrogramStruct {
    Matrix matrix;
    double timeStep;
    double frequencyStep;
} Spectrogram;

Spectrogram CreateSpectrogram(unsigned int nRows, unsigned int nCols);
void DestroySpectrogram(Spectrogram *spectrogram);

Spectrogram HardFilterSpectrogram(Spectrogram const *spectrogram, unsigned int numNewRows);
void NormalizeSpectrogram(Spectrogram *spectrogram);

void SaveSpectrogramToCSV(const char *filename, Spectrogram *spectrogram);

double GetDelay(Spectrogram const *spectrogram, double threshold);

void spectrogramTest();

#endif //C_PIANO_TRANSCRIPTION_SPECTROGRAM_H
