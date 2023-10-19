#include "domain_adaptation.h"
#include "dictionary.h"
#include "math.h"

#include "activations.h"

#include "dirent.h"

#include "stft.h"

void LoadSingleNoteDictionary(Dictionary* single_note_dictionary, char **piano_W, int numPianos, int midi_note, char* single_template_dir)
{
    int num_templates = -1;


    DIR *d;
    struct dirent *dir;
    d = opendir(single_template_dir);
    if (d) {
        while ((dir = readdir(d)) != NULL) {

            //            printf("Reading file: %s\n", dir->d_name);

            char delimiter[2] = "_";

            char filename[256] = "";
            strcpy(filename, dir->d_name);
            char *token = strtok(dir->d_name, delimiter);

            if(strcmp(token, "W") == 0){

                for(int i= 0; i < 4; i++){
                    token = strtok(NULL, delimiter);
                }

                int found_piano_in_templates = 0;

                //                printf("Checking if piano %s  is in template list\n", token);

                for (int i = 0; i < numPianos; i++) {
                    if (strcmp(piano_W[i], token) == 0) {
                        found_piano_in_templates = 1;
                        break;
                    }
                }

                if(found_piano_in_templates){

                    //                    printf("Piano %s is in template list\n", token);

                    for(int i= 0; i < 2; i++){
                        token = strtok(NULL, delimiter);
                    }


                    char* file_midi = strtok(token, ".");

                    //                    printf("Converting midi value %s to integer\n", token);

                    int file_midi_value = atoi(file_midi);

                    if(midi_note != file_midi_value){
                        //                        printf("This template does not correspond to the note we are looking for\n");

                    }else{
                        //                        printf("This template is valid and will now be loaded\n");


                        num_templates++;

                        char filepath[256];

                        sprintf(filepath, "%s%s", single_template_dir, filename);


                        int dims[3];

                        FILE *file = fopen(filepath, "r");

                        if (file == NULL) {
                            perror("Error opening file");

                            printf(filepath);
                            fflush(stdout);

                            exit(1);
                        }

                        //    int T;
                        //    int bins;
                        //    int notes;

                        double value;
                        int shape;

                        //    int index = 0;

                        for(int i = 0; i < 3; i++){
                            fscanf(file, "%d", &shape);
                            int ss = shape;
                            single_note_dictionary->shape[i] = ss;
                            printf("dict shape: %d\n", ss);
                        }

                        AllocateDictionaryMemory(single_note_dictionary);

                        for(int i = 0; i < single_note_dictionary->shape[0]; i++){
                            for(int j = 0; j < single_note_dictionary->shape[1]; j++){
                                for(int k = 0; k < single_note_dictionary->shape[2]; k++){
                                    fscanf(file, "%lf", &value);
                                    //                printf("%lf\n", value);
                                    single_note_dictionary->data[i][j][k] = value;
                                }
                            }
                        }

                        fclose(file);
                    }
                }
            }
            else{
                printf("File is not a template\n");
            }
        }
        closedir(d);
    }
}

int GetSingleNoteFromSpec(Spectrogram* note_spec)
{
    DynamicArray bins_summed_over_time = CreateDynamicArray(note_spec->matrix.rows);

    for(int row = 0; row < note_spec->matrix.rows; row++){
        double sum = 0;

        for(int col = 0; col < note_spec->matrix.cols; col++){
            sum += note_spec->matrix.array[row][col];
        }

        bins_summed_over_time.array[row] = sum;
    }

    int findamental_bin = 0;

    for(int row = 0; row < note_spec->matrix.rows; row++){
        if(bins_summed_over_time.array[row] > bins_summed_over_time.array[findamental_bin]){
            findamental_bin = row;
        }
    }

    DestroyDynamicArray(&bins_summed_over_time);

    printf("Fundamental bin established: %d\n", findamental_bin);

    double fundamental_frequency = findamental_bin * 22050 / 4096.0;

    printf("Fundamental frequency established: %f\n", fundamental_frequency);

    int midi_note = round(69 + 12 * log2(fundamental_frequency / 440.0));

    printf("Midi note established: %d\n", midi_note);

    return midi_note;
}

DynamicArray CalculateTemplateWeights(Spectrogram* spectrogram, int num_iterations){
    Matrix differences = CreateMatrix(spectrogram->matrix.rows, spectrogram->matrix.cols-1);

    int start_column;

    for(int col = 0; col < spectrogram->matrix.cols-1; col++){
        for(int row = 0; row < spectrogram->matrix.rows; row++){
            differences.array[row][col] = spectrogram->matrix.array[row][col+1] - spectrogram->matrix.array[row][col];

            if(differences.array[row][col] > 0.2){
                start_column = col;

                goto onset_detected;
            }            
        }
    }
//onset_not_detected: ;

    fprintf(stderr, "No onset detected during dictionary adaptation, threshold likely too high\n");
    fflush(stderr);
    exit(1);

onset_detected: ;

    printf("Onset detected\n");

    DestroyMatrix(&differences);

    int end_column = start_column + 15;

    printf("start: %d, end: %d\n", start_column, end_column);

    Spectrogram note_spec = CreateSpectrogram(spectrogram->matrix.rows, end_column - start_column);

//    Spectrogram  *note_spec_ptr = &note_spec;

    for(int col = start_column; col < end_column; col++){
        for(int row = 0; row < spectrogram->matrix.rows; row++){
            note_spec.matrix.array[row][col-start_column] = spectrogram->matrix.array[row][col];
        }
    }

    SaveSpectrogramToCSV("note_spec.csv", &note_spec);


    int midi_note = GetSingleNoteFromSpec(&note_spec);


    char *piano_W[8] = {"AkPnBcht", "AkPnBsdf", "AkPnCGdD", "AkPnStgb", "ENSTDkCl", "ENSTDkAm", "SptkBGAm", "StbgTGd2"};


    char single_template_dir[256] = "C:/Users/ruanb/OneDrive/Desktop/Piano Transcripton/Piano Transcription (C)/data_persisted/single_notes/csv/";


    Dictionary single_note_dictionary;
    single_note_dictionary.shape[0] = 10;
    single_note_dictionary.shape[1] = 4097;
    single_note_dictionary.shape[2] = 5;

    AllocateDictionaryMemory(&single_note_dictionary);

    printf("Single note dictionary memory allocated\n");

    printf("Dimensions: %d %d %d\n", single_note_dictionary.shape[0], single_note_dictionary.shape[1], single_note_dictionary.shape[2]);
    fflush(stdout);


    LoadSingleNoteDictionary(&single_note_dictionary, piano_W, 5, midi_note, single_template_dir);

    single_note_dictionary = HardFilterSpectrograms(&single_note_dictionary, 2000);

    printf("Single note dictionary memory allocated\n");

    printf("Dimensions: %d %d %d\n", single_note_dictionary.shape[0], single_note_dictionary.shape[1], single_note_dictionary.shape[2]);
    fflush(stdout);


    Matrix weights_activation = ComputeActivations(&note_spec, 15, &single_note_dictionary, "constant");

    // get means along axis 1

    printf("Activations done\n");

    DestroySpectrogram(&note_spec);
    DestroyDictionary(&single_note_dictionary);


    fflush(stdout);

    DynamicArray weights_mean = CreateDynamicArray(weights_activation.rows);

    for(int row = 0; row < weights_activation.rows; row++){
        double sum = 0;

        for(int col = 0; col < weights_activation.cols; col++){
            sum += weights_activation.array[row][col];
        }

        weights_mean.array[row] = sum / weights_activation.cols;
    }

    DestroyMatrix(&weights_activation);

    double total_sum = 0;

    for(int row = 0; row < weights_mean.size; row++){
        total_sum += weights_mean.array[row];
    }


    for(int row = 0; row <  weights_mean.size; row++){
        weights_mean.array[row] /= total_sum;
        printf("%lf ", weights_mean.array[row]);
    }

    printf("\n");

    fflush(stdout);

    return weights_mean;
}


void DomainAdaptationTest()
{
//    const char* piano_W[] = {"AkPnBcht", "AkPnBsdf", "AkPnCGdD", "AkPnStgb", "ENSTDkCl", "ENSTDkAm", "SptkBGAm", "StbgTGd2"};

////    const char *filename = "MAPS_MUS-alb_se3_AkPnBcht.wav";
//        const char *filename = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano transcription\\MAPS\\AkPnBcht\\MUS\\MAPS_MUS-alb_se3_AkPnBcht.wav";

//    double time_limit = 10;
//    int iterations = 10;

//    WavFile wav = ReadWav(filename);

//    DynamicArray mono = StereoToMono(&wav, "average");

//    Spectrogram spec = STFT(&mono, 4096, 882, 8192, time_limit, 44100);

//    printf("Starting template weight calc\n");

//    Dictionary dictionaries[5];

//    for(int i = 0; i < 5; i++){
//        dictionaries[i] = GetDictionary(piano_W[i]);
//    }

//    DynamicArray weights = CalculateTemplateWeights(&spec, iterations);

//    printf("Template weight calc done\n");

//    Dictionary adapted_dictionary = AdaptDictionary( dictionaries, &weights);

//    printf("Adapted dimensions: %d %d %d\n", adapted_dictionary.shape[0], adapted_dictionary.shape[1], adapted_dictionary.shape[2]);
//    fflush(stdout);

}

Dictionary AdaptDictionary(Dictionary* dictionaries, DynamicArray* weights){
    Dictionary adapted_dictionary;

    adapted_dictionary.shape[0] = dictionaries[0].shape[0];
    adapted_dictionary.shape[1] = dictionaries[0].shape[1];
    adapted_dictionary.shape[2] = dictionaries[0].shape[2];

    AllocateDictionaryMemory(&adapted_dictionary);

    for(int i = 0; i < adapted_dictionary.shape[2]; i++){
        for(int row = 0; row < adapted_dictionary.shape[0]; row++){
            for(int col = 0; col < adapted_dictionary.shape[1]; col++){
                double sum = 0;

                for(int j = 0; j < weights->size; j++){
                    sum += weights->array[j] * dictionaries[j].data[row][col][i];
                }

                adapted_dictionary.data[row][col][i] = sum;
            }
        }
    }

    return adapted_dictionary;
}


Dictionary GetBestDictionaryForArray(DynamicArray *array)
{
    const char* piano_W[] = {"AkPnBcht", "AkPnBsdf", "AkPnCGdD", "AkPnStgb", "ENSTDkCl", "ENSTDkAm", "SptkBGAm", "StbgTGd2"};

    //    const char *filename = "MAPS_MUS-alb_se3_AkPnBcht.wav";

    double time_limit = 10;
    int iterations = 5;


    Spectrogram spec = STFT(array, 4096, 882, 8192, time_limit, 44100);
    spec = HardFilterSpectrogram(&spec, 2000);

    printf("Starting template weight calc\n");

    Dictionary dictionaries[5];

    for(int i = 0; i < 5; i++){
        dictionaries[i] = GetDictionary(piano_W[i]);
    }

    DynamicArray weights = CalculateTemplateWeights(&spec, iterations);

    printf("Template weight calc done\n");

    Dictionary adapted_dictionary = AdaptDictionary( dictionaries, &weights);

    printf("Adapted dimensions: %d %d %d\n", adapted_dictionary.shape[0], adapted_dictionary.shape[1], adapted_dictionary.shape[2]);
    fflush(stdout);

    DestroySpectrogram(&spec);

    for(int i = 0; i < 5; i++){
        DestroyDictionary(&dictionaries[i]);
    }

    return adapted_dictionary;

}
