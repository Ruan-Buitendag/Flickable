#ifndef FULL_TRANSCRIPTION_H
#define FULL_TRANSCRIPTION_H


#include "matrix.h"
#include "dynamicarray.h"

Matrix full_transcription_from_wav(const char* songFile, const char* transcriptionFile);

Matrix full_transcription_from_array(const DynamicArray * array, const char* transcriptionFile);


void test();


#endif // FULL_TRANSCRIPTION_H
