//
// Created by ruanb on 9/5/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_DYNAMICARRAY_H
#define C_PIANO_TRANSCRIPTION_DYNAMICARRAY_H

#ifdef __cplusplus
extern "C" {
#endif




struct DynamicArrayStruct {
    double *array;
    unsigned long size;
};

typedef struct DynamicArrayStruct DynamicArray;

DynamicArray CreateDynamicArray(unsigned long size);
void DestroyDynamicArray(DynamicArray *dynamicArray);

void FillDynamicArray(DynamicArray *dynamicArray, double value);
void CopyDynamicArray(DynamicArray *dest, DynamicArray *src);
DynamicArray AppendDynamicArray(DynamicArray *dest, DynamicArray *src);
double Sum(DynamicArray const *dynamicArray);

void PrintDynamicArray(DynamicArray const *dynamicArray);

void SaveArrayToCSV(char *filename, DynamicArray *dynamicArray);


void dynamicArrayTest();


#ifdef __cplusplus
}
#endif

#endif //C_PIANO_TRANSCRIPTION_DYNAMICARRAY_H
