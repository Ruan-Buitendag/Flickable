#ifndef FULL_TRANSCRIPTION_H
#define FULL_TRANSCRIPTION_H


#include "matrix.h"
#include "dynamicarray.h"
#include "dictionary.h"

Matrix full_transcription_from_wav(const char* songFile, const char* transcriptionFile, Dictionary* dictionary);

Matrix full_transcription_from_array(DynamicArray * array, const char* transcriptionFile, Dictionary* dictionary);

Matrix transcribe(Spectrogram* songSpectrogram, Dictionary* dictionary, int transcriptionIterations);

Matrix transcribe_array(DynamicArray* array, const char * transcriptionFile,int transcriptionIterations);

Matrix transcribe_wav(const char * wavFile, const char * transcriptionFile, int transcriptionIterations);

void test();

#endif // FULL_TRANSCRIPTION_H
