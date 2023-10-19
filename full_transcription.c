#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamicarray.h"
#include "stft.h"
#include "math.h"
#include "evaluation.h"


#include "wav.h"
#include "activations.h"
#include "activation_postprocessing.h"

#include "full_transcription.h"

#include <time.h>

#include "omp.h"

#include "domain_adaptation.h"


void test() {


    printf("Max threads: %d\n", omp_get_max_threads());

    #pragma omp parallel
    {

        printf("%d : ", omp_get_num_threads());

        printf("%d\n", omp_get_thread_num());

    }
}

Matrix transcribe(Spectrogram* songSpectrogram, Dictionary *dictionary, int transcriptionIterations)
{
    clock_t start_time, end_time;

    start_time = clock();

    Matrix activations = ComputeActivations(songSpectrogram, transcriptionIterations,  dictionary, "gauss");

    end_time = clock();

    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time for activations: %.2f seconds\n", elapsed_time);

    double threshold = GetThreshold(&activations);

    Matrix notes = TranscribeNotesFromActivations(&activations, threshold, 0.02);

    DestroyMatrix(&activations);

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

    DestroyMatrix(&notes);


    for (int i = 0; i < finalIndex; i++) {
        finalNotes.array[i][0] -= est_min;
    }

    SaveMatrixToCSV("finalnotes.csv", &finalNotes);

    return finalNotes;
}

Matrix transcribe_array(DynamicArray *array, const char * transcriptionFile, int transcriptionIterations)
{

    double time_limit = 5;

    Spectrogram spec = STFT(array, 4096, 882, 8192, time_limit, 44100);
    Spectrogram filtered = HardFilterSpectrogram(&spec, 1500);

    DestroySpectrogram(&spec);

    SaveSpectrogramToCSV("spectrogram.csv", &filtered);


    DynamicArray arrayCopy = CreateDynamicArray(array->size);
    CopyDynamicArray(&arrayCopy, array);


    Dictionary dictionary = GetBestDictionaryForArray(&arrayCopy);

    DestroyDynamicArray(&arrayCopy);

    dictionary = HardFilterSpectrograms(&dictionary, 1500);
    NormaliseDictionary(&dictionary);

    double delay = GetDelay(&filtered, 0.05);

    printf("delay: %f\n", delay);

    Matrix estimated_notes = transcribe(&filtered, &dictionary, transcriptionIterations);

    DestroySpectrogram(&filtered);
    DestroyDictionary(&dictionary);

    if(strcmp(transcriptionFile, "") != 0)
    {
        Matrix reference_notes = LoadRefsFromFile(transcriptionFile, time_limit - delay);

        EvaluateTranscription(&reference_notes, &estimated_notes);

        DestroyMatrix(&reference_notes);
    }

    return estimated_notes;
}

Matrix transcribe_wav(const char *wavFile, const char *transcriptionFile, int transcriptionIterations)
{
    WavFile wav = ReadWav(wavFile);

    DynamicArray mono = StereoToMono(&wav, "average");
//    NormaliseChannel(&mono);


    Matrix estimated_notes = transcribe_array(&mono, transcriptionFile, transcriptionIterations);

    DestroyDynamicArray(&mono);

    return estimated_notes;
}
