//
// Created by ruanb on 9/5/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_STFT_H
#define C_PIANO_TRANSCRIPTION_STFT_H

#include "dynamicarray.h"
#include "spectrogram.h"
#include "complex.h"

Spectrogram STFT(DynamicArray *x, int windowSize, int hopSize, int fftSize, double time_limit, int sampling_rate);

DynamicArray HanningWindow(int windowSize);

void fft(double const *x, int N, double complex *result);
void rfft(double const *x, int N, double *result);


void stftTest();



#endif //C_PIANO_TRANSCRIPTION_STFT_H
