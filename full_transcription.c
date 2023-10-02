#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamicarray.h"
#include "stft.h"
#include "math.h"
#include "evaluation.h"


#include "wav.h"
#include "activations.h"
#include "note_transcription.h"

#include "full_transcription.h"


void test() {
//    WavFile wav = ReadWav(
//            "C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano transcription/MAPS/AkPnBcht/ISOL/NO/MAPS_ISOL_NO_F_S0_M23_AkPnBcht.wav");

//    DynamicArray mono = StereoToMono(&wav, "average");

//    SaveArrayToCSV("mono.csv", &mono);

//    Spectrogram spec = STFT(&mono, 4096, 882, 8192, 1, 44100);

//    SaveSpectrogramToCSV("testSpec.csv", &spec);

    int a = 0;

    printf("aaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

Matrix full_transcription(){
    // Define the parameters

//    dictionaryTest();
//    stftTest();
//    TestActivations();
//    test();
//    GraphTest();
//    matrixTest();



    const char *filename = "MAPS_MUS-alb_se3_AkPnBcht.wav";
//    const char *filename = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.wav";


    double time_limit = 6;
    int iterations = 20;

    WavFile wav = ReadWav(filename);

    DynamicArray mono = StereoToMono(&wav, "average");

    Spectrogram spec = STFT(&mono, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

    double delay = GetDelay(&filtered, 0.05);

    printf("delay: %f\n", delay);

    Matrix activations = GetActivations(filename, time_limit, iterations);
//    Matrix activations = LoadMatrixFromCSV("H_to_test_F1calc.csv");

    double threshold = GetThreshold(&activations);

    Matrix notes = TranscribeNotesFromActivations(&activations, threshold, 0.02);

    int finalIndex = 0;

    while (true) {
        if (notes.array[finalIndex][1] == 0) {
            break;
        }
        finalIndex++;
    }

    Matrix finalNotes = CreateMatrix(finalIndex, 2);

    double est_min = 99;

    for (int i = 0; i < finalIndex; i++) {
        if (notes.array[i][0] < est_min) {
            est_min = notes.array[i][0];
        }

        finalNotes.array[i][0] = notes.array[i][0];
        finalNotes.array[i][1] = notes.array[i][1];
    }


    for (int i = 0; i < finalIndex; i++) {
        finalNotes.array[i][0] -= est_min;
    }



    Matrix refs = LoadRefsFromFile(
            "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.txt",
            time_limit - delay);
//            time_limit );

    double ref_min = 99;

    for (int i = 0; i < refs.rows; i++) {
        if (refs.array[i][0] < ref_min) {
            ref_min = refs.array[i][0];
        }
    }

    for (int i = 0; i < refs.rows; i++) {
        refs.array[i][0] -= ref_min;
    }

    SaveMatrixToCSV("finalnotes.csv", &finalNotes);


    EvaluateTranscription(&refs, &finalNotes);


    return finalNotes;
}



