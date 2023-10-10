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

#include <time.h>

#include "omp.h"


void test() {


    printf("Max threads: %d\n", omp_get_max_threads());

    #pragma omp parallel
    {

        printf("%d : ", omp_get_num_threads());

        printf("%d\n", omp_get_thread_num());

    }
}

Matrix full_transcription_from_wav(const char* songFile, const char* transcriptionFile){
    // Define the parameters

//    dictionaryTest();
//    stftTest();
//    TestActivations();
//    test();
//    GraphTest();
//    matrixTest();
//    test();

    clock_t start_time, end_time;

    start_time = clock();

//    const char *filename = "MAPS_MUS-alb_se3_AkPnBcht.wav";
//    const char *filename = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.wav";


    double time_limit = 2;
    int iterations = 20;

    WavFile wav = ReadWav(songFile);

    DynamicArray mono = StereoToMono(&wav, "average");

    Spectrogram spec = STFT(&mono, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

    double delay = GetDelay(&filtered, 0.05);

    printf("delay: %f\n", delay);

    Matrix activations = GetActivations(songFile, time_limit, iterations, true);
//    Matrix activations = LoadMatrixFromCSV("H_to_test_F1calc.csv");

    end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time for activations: %.2f seconds\n", elapsed_time);

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


//    Matrix refs = LoadRefsFromFile(
//            "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.txt",
//            time_limit - delay);

    Matrix refs = LoadRefsFromFile(transcriptionFile, time_limit - delay);

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




Matrix full_transcription_from_array(const DynamicArray *array, const char *transcriptionFile)
{
    clock_t start_time, end_time;

    start_time = clock();

    //    const char *filename = "MAPS_MUS-alb_se3_AkPnBcht.wav";
    //    const char *filename = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.wav";


    double time_limit = 2;
    int iterations = 20;

    // TODO: duplicate code in getactivations

    Spectrogram spec = STFT(array, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

    double delay = GetDelay(&filtered, 0.05);

    printf("delay: %f\n", delay);

    Matrix activations = GetActivations(array, time_limit, iterations, true);
    //    Matrix activations = LoadMatrixFromCSV("H_to_test_F1calc.csv");

    end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time for activations: %.2f seconds\n", elapsed_time);

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


    //    Matrix refs = LoadRefsFromFile(
    //            "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.txt",
    //            time_limit - delay);

    Matrix refs = LoadRefsFromFile(transcriptionFile, time_limit - delay);

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
