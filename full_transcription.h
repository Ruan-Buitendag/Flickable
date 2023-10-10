#ifndef FULL_TRANSCRIPTION_H
#define FULL_TRANSCRIPTION_H


#include "matrix.h"
#include "dynamicarray.h"
#include "dictionary.h"

Matrix full_transcription_from_wav(const char* songFile, const char* transcriptionFile, Dictionary* dictionary);

Matrix full_transcription_from_array(DynamicArray * array, const char* transcriptionFile, Dictionary* dictionary);

void test();

#endif // FULL_TRANSCRIPTION_H
