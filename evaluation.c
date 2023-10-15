//
// Created by ruanb on 9/29/2023.
//

#include <math.h>
#include "evaluation.h"
#include "stdio.h"

Graph CreateGraph(int maxRef, int maxEstCount) {
//    Matrix result = CreateMatrix(maxRef, maxEstCount);
    Graph result;

    result.array = (IntArray *) calloc(maxRef, sizeof(IntArray));

    for (int i = 0; i < maxRef; i++) {
        result.array[i] = CreateIntArray(maxEstCount);
    }

    result.left = maxRef;
    result.right = maxEstCount;

    return result;
}

void DestroyGraph(Graph *graph) {
    for (int i = 0; i < graph->left; i++) {
        DestroyIntArray(&graph->array[i]);
    }

    free(graph->array);
    graph->array = NULL;
}


void AddEdge(Graph *graph, int ref, int est) {
    int currIndex = graph->array[ref].size;

    graph->array[ref].array[currIndex] = est;

    graph->array[ref].size += 1;
}

IntArray CreateIntArray(int capacity) {
    IntArray result;

    result.array = (int *) calloc(capacity, sizeof(int));
    result.size = 0;
    result.capacity = capacity;

    return result;
}


void DestroyIntArray(IntArray *array) {
    free(array->array);
    array->array = NULL;
}

bool dfs(Graph *graph, int u, IntArray *visited, IntArray *matchR, IntArray *matchL) {
    for (int v_index = 0; graph->array[u].size; v_index++) {
        int v = graph->array[u].array[v_index];

        if (visited->array[v]) {
            continue;
        }

        visited->array[v] = 1;

        if (matchR->array[v] == -1 || dfs(graph, matchR->array[v], visited, matchR, matchL)) {
            matchL->array[u] = v;
            matchR->array[v] = u;

            return true;
        }
    }

    return false;
}

int MaxBipartiteMatching(Graph *graph) {
    IntArray leftMatch = CreateIntArray(graph->left);
    IntArray rightMatch = CreateIntArray(graph->right);

    for (int i = 0; i < graph->left; i++) {
        leftMatch.array[i] = -1;
    }
    for (int i = 0; i < graph->right; i++) {
        rightMatch.array[i] = -1;
    }

    int maxMatching = 0;

    for (int u = 0; u < graph->left; u++) {
        IntArray visited = CreateIntArray(graph->left);

        if (dfs(graph, u, &visited, &rightMatch, &leftMatch)) {
            maxMatching++;
        }

        DestroyIntArray(&visited);
    }

    DestroyIntArray(&leftMatch);
    DestroyIntArray(&rightMatch);

    return maxMatching;
}

void GraphTest() {
    Matrix refs = LoadRefsFromFile(
            "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.txt",
            5);

    SaveMatrixToCSV("refs.csv", &refs);
}

// TODO: optimize for loops
void EvaluateTranscription(Matrix *ref, Matrix *est) {
    Matrix onset_distances = CreateMatrix(ref->rows, est->rows);

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            onset_distances.array[i][j] = round(fabs(ref->array[i][0] - est->array[j][0]) * 10000) / 10000;
        }
    }

    Matrix onset_hit_matrix = CreateMatrix(ref->rows, est->rows);

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            onset_hit_matrix.array[i][j] = onset_distances.array[i][j] <= 0.05;
        }
    }

    DestroyMatrix(&onset_distances);

    Matrix pitch_distances = CreateMatrix(ref->rows, est->rows);

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            pitch_distances.array[i][j] = fabs(1200 * (log2(ref->array[i][1]) - log2(est->array[j][1])));
        }
    }

    Matrix pitch_hit_matrix = CreateMatrix(ref->rows, est->rows);

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            pitch_hit_matrix.array[i][j] = pitch_distances.array[i][j] <= 50;
        }
    }

    DestroyMatrix(&pitch_distances);

    Matrix hits = CreateMatrix(ref->rows, est->rows);

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            hits.array[i][j] = onset_hit_matrix.array[i][j] * pitch_hit_matrix.array[i][j];
        }
    }

    DestroyMatrix(&onset_hit_matrix);
    DestroyMatrix(&pitch_hit_matrix);

    int ref_i[1000];
    int est_i[1000];
    int counter = 0;

    for (int i = 0; i < ref->rows; i++) {
        for (int j = 0; j < est->rows; j++) {
            if (hits.array[i][j]) {
                ref_i[counter] = i;
                est_i[counter] = j;
                counter++;
            }
        }
    }

    DestroyMatrix(&hits);

    Graph graph = CreateGraph(1000, 1000);

    for (int i = 0; i < counter; i++) {
        AddEdge(&graph, ref_i[i], est_i[i]);
    }

    int maxMatching = MaxBipartiteMatching(&graph);


    double precision;

    if(est->rows == 0){
        precision = 0;
    } else {
        precision = (double)maxMatching / est->rows;
    }


    double recall;

    if(ref->rows == 0){
        recall = 0;
    } else {
        recall = (double)maxMatching / ref->rows;
    }

    double f1;

    if(precision > 0 && recall > 0){
        f1 = 2 * precision * recall / (precision + recall);
    } else {
        f1 = 0;
    }

    int falsePositives = est->rows - maxMatching;
    int falseNegatives = ref->rows - maxMatching;

    printf("True positives: %d\n", maxMatching);
    printf("False positives: %d\n", falsePositives);
    printf("False negatives: %d\n", falseNegatives);
    printf("Precision: %f\n", precision);
    printf("Recall: %f\n", recall);
    printf("F1: %f\n", f1);

    DestroyGraph(&graph);

}

Matrix LoadRefsFromFile(const char *filename, double time_limit) {
    FILE *file = fopen(filename, "r"); // Replace "your_file.txt" with the actual file name

    if (file == NULL) {
        printf("Failed to open the file.\n");
        exit(1);
    }

    int rows = 0;
    char line[100];
    double temp;

    bool firstPass = true;


    fscanf(file, "%s %s %s", line, line, line); // Skip the first line (header)

    // First pass to determine the number of rows
    while(true) {
        double onset;
        double pitch;

        fscanf(file, "%lf %lf %lf", &onset, &temp, &pitch);

        if(firstPass){
            time_limit += onset;
            firstPass = false;
        }

        if(onset > time_limit){
            break;
        }

        rows++;
    }

    // Reset the file pointer to the beginning of the file
    fseek(file, 0, SEEK_SET);
    fscanf(file, "%s %s %s", line, line, line); // Skip the first line (header)


    Matrix refs = CreateMatrix(rows, 2);


    for (int row = 0; row < rows; row++) {
        double onset;
        double pitch;

        fscanf(file, "%lf %lf %lf", &onset, &temp, &pitch);

        if(onset > time_limit){
            break;
        }

        refs.array[row][0] = onset;
        refs.array[row][1] = pitch;
    }

    double ref_min = 99;

    for (int i = 0; i < refs.rows; i++) {
        if (refs.array[i][0] < ref_min) {
            ref_min = refs.array[i][0];
        }
    }

    for (int i = 0; i < refs.rows; i++) {
        refs.array[i][0] -= ref_min;
    }


    return refs;
}


