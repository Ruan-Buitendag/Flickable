//
// Created by ruanb on 9/30/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_TRANSCRIPTION_H
#define C_PIANO_TRANSCRIPTION_TRANSCRIPTION_H

#include "matrix.h"

Matrix TranscribeNotesFromActivations(Matrix const * activations, double threshold, double timeStep);

double GetThreshold(Matrix const * activations);

void testTranscription();

#endif //C_PIANO_TRANSCRIPTION_TRANSCRIPTION_H
