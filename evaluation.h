//
// Created by ruanb on 9/29/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_EVALUATION_H
#define C_PIANO_TRANSCRIPTION_EVALUATION_H

#include <stdbool.h>
#include "matrix.h"
#include "stdlib.h"

typedef struct {
    int *array;
    int size;
    int capacity;
} IntArray;


typedef struct {
    IntArray *array;
    int left;
    int right;
} Graph;



Graph CreateGraph(int maxRef, int maxEstCount);
IntArray CreateIntArray(int capacity);
void DestroyIntArray(IntArray *array);
void DestroyGraph(Graph *graph);

void AddEdge(Graph *graph, int ref, int est);

bool dfs(Graph * graph, int u, IntArray* visited, IntArray* matchR, IntArray* matchL);

int MaxBipartiteMatching(Graph *graph);

void GraphTest();

void EvaluateTranscription(Matrix *ref, Matrix *est);

Matrix LoadRefsFromFile(const char * filename, double time_limit);



#endif //C_PIANO_TRANSCRIPTION_EVALUATION_H
