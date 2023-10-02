//
// Created by ruanb on 9/5/2023.
//

#include "dynamicarray.h"
#include "stdlib.h"
#include "stdio.h"

DynamicArray CreateDynamicArray(unsigned long size) {
    DynamicArray dynamicArray;
//    dynamicArray.array = (double *) malloc(size * sizeof(double));
    dynamicArray.array = (double *) calloc(size, sizeof(double));

    if (dynamicArray.array == NULL) {
        fprintf(stderr, "Error allocating memory. Exiting...\n");
        exit(1);
    }

    dynamicArray.size = size;
    return dynamicArray;
}

void DestroyDynamicArray(DynamicArray *dynamicArray) {
    if (dynamicArray->array == NULL) {
        fprintf(stderr,
                "The dynamic array pointer is NULL. The dynamic array was likely already destroyed. Exiting...\n");
        exit(1);
    }

//    printf("Destroying dynamic array...\n");
    free(dynamicArray->array);
//    printf("Dynamic array destroyed.\n");
    dynamicArray->array = NULL;
    dynamicArray->size = 0;
}

void CopyDynamicArray(DynamicArray *dest, DynamicArray *src) {
    if (dest->size < src->size) {
        fprintf(stderr, "The destination array is too small to hold the source array. Reallocating memory...\n");
        dest->array = (double *) realloc(dest->array, src->size * sizeof(double));


        if (dest->array == NULL) {
            fprintf(stderr, "Error reallocating memory. Exiting...\n");
            exit(1);
        }

    }

    dest->size = src->size;

    for (int i = 0; i < src->size; i++) {
        dest->array[i] = src->array[i];
    }
}

DynamicArray AppendDynamicArray(DynamicArray *first, DynamicArray *second) {
    unsigned int newSize = first->size + second->size;

    // Allocate memory for the new array
    DynamicArray newArray = CreateDynamicArray(newSize);

    // Copy elements from the first array to the new array
    for (int i = 0; i < first->size; i++) {
        newArray.array[i] = first->array[i];
    }

    // Copy elements from the second array to the new array
    for (int i = 0; i < second->size; i++) {
        newArray.array[first->size + i] = second->array[i];
    }

    return newArray;
}

void FillDynamicArray(DynamicArray *dynamicArray, double value) {
    for (int i = 0; i < dynamicArray->size; i++) {
        dynamicArray->array[i] = value;
    }
}

double Sum(DynamicArray const *dynamicArray) {
    double sum = 0;
    for (int i = 0; i < dynamicArray->size; i++) {
        sum += dynamicArray->array[i];
    }

    return sum;
}

void PrintDynamicArray(const DynamicArray *dynamicArray) {
    for (int j = 0; j < dynamicArray->size; j++) {
        printf("%d : %f\n", j, dynamicArray->array[j]);
    }
    printf("Printed padding\n");
    fflush(stdout);
}

void dynamicArrayTest() {
    DynamicArray aa = CreateDynamicArray(5);

    aa.array[0] = 1;
    aa.array[1] = 2;
    aa.array[2] = 3;
    aa.array[3] = 4;
    aa.array[4] = 5;

//    print the contents of aa
    for (int i = 0; i < aa.size; i++) {
        printf("%f\n", aa.array[i]);
    }

    DestroyDynamicArray(&aa);
}

void SaveArrayToCSV(char *filename, DynamicArray *dynamicArray) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error opening file %s. Exiting...\n", filename);
        exit(1);
    }

    for (int i = 0; i < dynamicArray->size; i++) {
        fprintf(fp, "%.8f\n", dynamicArray->array[i]);
    }

    fclose(fp);

}












