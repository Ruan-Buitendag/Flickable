//
// Created by ruanb on 9/30/2023.
//

#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "activation_postprocessing.h"

Matrix TranscribeNotesFromActivations(const Matrix *activations, double threshold, double time_step) {
//    Matrix notes = CreateMatrix(500, 3);
    Matrix notes = CreateMatrix(1000, 2);

    int note_count = 0;

    int midi_codebook[88];

    for (int i = 0; i < 88; i++) {
        midi_codebook[i] = i + 21;
    }

    bool note_presence = false;

    SaveMatrixToCSV("unsmoothed_activation.csv", activations);

    double current_pitch = 0;
    double current_onset = 0;
    int sliding_window_width = 10;

//    TODO: midi file creation

    Matrix smoothed_activations = CreateMatrix(activations->rows, activations->cols);

    for (int i = 0; i < activations->cols; i++) {
        if (i - sliding_window_width < 0 || i + sliding_window_width + 1 > activations->cols) {
            int d = floor(fmin(i, activations->cols - i));

            for (int j = 0; j < activations->rows; j++) {
                double sum = 0;
                int count = 0;
                for (int l = 0; l < activations->rows; l++) {
                    for (int k = i - d; k < i + d + 1; k++) {
                        if ((k >= 0 && k < activations->cols) && (l >= 0 && l < activations->rows)) {
                            sum += activations->array[l][k];
                            count++;
                        }
                    }
                }
                smoothed_activations.array[j][i] = sum / count;
            }
        } else {
            for (int j = 0; j < activations->rows; j++) {
                double sum = 0;
                int count = 0;
                for (int k = i - sliding_window_width; k < i + sliding_window_width + 1; k++) {
                    if (k >= 0 && k < activations->cols) {
                        sum += activations->array[j][k];
                        count++;
                    }
                }

                smoothed_activations.array[j][i] = sum / count;
            }
        }
    }

    for (int note_index = 0; note_index < smoothed_activations.rows; note_index++) {
        if (note_presence)
            note_presence = false;

        for (int time_index = 0; time_index < smoothed_activations.cols; time_index++) {
            bool minimal_sustain_condition = (
                    activations->array[note_index][time_index] - smoothed_activations.array[note_index][time_index] >
                    threshold);

            if(minimal_sustain_condition){
                if(!note_presence){
                    current_pitch = midi_codebook[note_index];
                    current_onset = time_index * time_step;
                    note_presence = true;
                }
            }
            else{
                if(note_presence){
//                    double current_offset = time_index * time_step;
                    notes.array[note_count][0] = current_onset;
//                    notes.array[note_count][1] = current_offset;
//                    notes.array[note_count][2] = current_pitch;
                    notes.array[note_count][1] = current_pitch;
                    note_count++;
//                    TODO: writing notes to midi file

                    note_presence = false;
                }
            }
        }

    }

    return notes;
}

void testTranscription() {
    Matrix X = CreateMatrix(88, 100);

    for (int i = 0; i < X.rows; i++) {
        for (int j = 0; j < X.cols; j++) {
            X.array[i][j] = i * j % 100;
        }
    }

    Matrix notes = TranscribeNotesFromActivations(&X, 0, 0.08);
    SaveMatrixToCSV("notes.csv", &notes);
}

double GetThreshold(const Matrix *activations) {
    //        Calculate variance of all activation entires above 0.01
    double mean = 0;
    int count = 0;
    for (int i = 0; i < activations->rows; i++) {
        for (int j = 0; j < activations->cols; j++) {
            if (activations->array[i][j] > 0.01) {
                mean += activations->array[i][j];
                count++;
            }
        }
    }

    mean /= count;

    double variance = 0;

    for (int i = 0; i < activations->rows; i++) {
        for (int j = 0; j < activations->cols; j++) {
            if (activations->array[i][j] > 0.01) {
                variance += pow(activations->array[i][j] - mean, 2);
            }
        }
    }

    variance /= count - 1;

    double threshold = 9.8125 * variance + 0.0318;

    printf("Threshold: %f\n", threshold);

    return threshold;
}
