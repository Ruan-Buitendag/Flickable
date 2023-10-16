//
// Created by ruanb on 9/4/2023.
//

#include "dictionary.h"

void LoadDictionary(Dictionary *dictionary, char *filename) {
    hid_t file_id;
    hid_t dataset_id;
    hid_t dataspace_id;
    herr_t status;
    hsize_t dims[3];

    printf("Opening file: %s\n", filename);

    // Open the HDF5 file
    file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "Error opening the file.\n");
        exit(1);
    }

    // Open the dataset
    dataset_id = H5Dopen2(file_id, "dictionary", H5P_DEFAULT);
    if (dataset_id < 0) {
        fprintf(stderr, "Error opening the dataset.\n");
        H5Fclose(file_id);
        exit(1);
    }

    // Get the dataspace
    dataspace_id = H5Dget_space(dataset_id);
    if (dataspace_id < 0) {
        fprintf(stderr, "Error getting the dataspace.\n");
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        exit(1);
    }

    // Get the dimensions of the dataset
    H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

    for (int dim = 0; dim < 3; dim++) {
        dictionary->shape[dim] = dims[dim];
    }

    double *flattened_dictionary = (double *) malloc(
            dictionary->shape[0] * dictionary->shape[1] * dictionary->shape[2] * sizeof(double));

    status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, flattened_dictionary);
    if (status < 0) {
        fprintf(stderr, "Error reading the dataset.\n");
        H5Sclose(dataspace_id);
        H5Dclose(dataset_id);
        H5Fclose(file_id);
        exit(1);
    }

    H5Sclose(dataspace_id);
    H5Dclose(dataset_id);
    H5Fclose(file_id);

    AllocateDictionaryMemory(dictionary);

    int currentPos = 0;

    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                dictionary->data[i][j][k] = flattened_dictionary[currentPos];
                currentPos++;
            }
        }
    }

    free(flattened_dictionary);
}

void AllocateDictionaryMemory(Dictionary *dictionary) {
    dictionary->data = (double ***) malloc(dictionary->shape[0] * sizeof(double **));
    for (int i = 0; i < dictionary->shape[0]; i++) {
        dictionary->data[i] = (double **) malloc(dictionary->shape[1] * sizeof(double *));
        for (int j = 0; j < dictionary->shape[1]; j++) {
            dictionary->data[i][j] = (double *) malloc(dictionary->shape[2] * sizeof(double));
        }
    }
}

void PrintDictionary(Dictionary *dictionary) {
    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                printf("%f ", dictionary->data[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void dictionaryTest() {
    const char *dictionary_directory = "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano Transcription (C)\\data_persisted\\dictionaries\\";
    const char *piano_W[3] = {"AkPnBsdf", "AkPnStgb", "AkPnBcht"};

    Dictionary dictionaries[3];

    // Iterate through piano_W
    for (int i = 0; i < 3; i++) {
        char W_persisted_name[256]; // Adjust the size as needed

        // Create the formatted string
        snprintf(W_persisted_name, sizeof(W_persisted_name),
                 "%sconv_dict_piano_%s.h5",
                 dictionary_directory,
                 piano_W[i]);

        LoadDictionary(&dictionaries[i], W_persisted_name);
    }

//    Spectrogram aa = GetSpectrogramFromDictionary(&dictionaries[0], 0);


//    SaveSpectrogramToCSV("spectest.csv", &aa);

}

Dictionary HardFilterSpectrograms(Dictionary *dictionary, unsigned int numNewRows) {
    Dictionary filtered;

    filtered.shape[0] = dictionary->shape[0];
    filtered.shape[1] = numNewRows;
    filtered.shape[2] = dictionary->shape[2];

    AllocateDictionaryMemory(&filtered);

    for (int i = 0; i < filtered.shape[0]; i++) {
        for (int j = 0; j < filtered.shape[1]; j++) {
            for (int k = 0; k < filtered.shape[2]; k++) {
                filtered.data[i][j][k] = dictionary->data[i][j][k];
            }
        }
    }

    return filtered;
}

Spectrogram GetSpectrogramFromDictionary(Dictionary const *dictionary, unsigned int axis, unsigned int index) {
    Spectrogram noteSpectrogram;

    if (axis == 0) {
        noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[2]);

        for (int r = 0; r < noteSpectrogram.matrix.rows; r++) {
            for (int c = 0; c < noteSpectrogram.matrix.cols; c++) {
                noteSpectrogram.matrix.array[r][c] = dictionary->data[index][r][c];
            }
        }
    } else if (axis == 1) {
//        Spectrogram noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);
        fprintf(stderr, "GetSpectrogramFromDictionary: axis == 1 not implemented yet\n");
        exit(1);
    } else if (axis == 2) {
        noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);

        for (int r = 0; r < noteSpectrogram.matrix.rows; r++) {
            for (int c = 0; c < noteSpectrogram.matrix.cols; c++) {
                noteSpectrogram.matrix.array[r][c] = dictionary->data[c][r][index];
            }
        }
    } else {
        fprintf(stderr, "GetSpectrogramFromDictionary: axis must be 0, 1 or 2\n");
        exit(1);
    }


    return noteSpectrogram;
}


Matrix GetMatrixFromDictionary(const Dictionary *dictionary, unsigned int axis, unsigned int index) {
    Matrix slice;

    if (axis == 0) {
        slice = CreateMatrix(dictionary->shape[1], dictionary->shape[2]);

        for (int r = 0; r < slice.rows; r++) {
            for (int c = 0; c < slice.cols; c++) {
                slice.array[r][c] = dictionary->data[index][r][c];
            }
        }
    } else if (axis == 1) {
//        Spectrogram noteSpectrogram = CreateSpectrogram(dictionary->shape[1], dictionary->shape[0]);
        fprintf(stderr, "GetSpectrogramFromDictionary: axis == 1 not implemented yet\n");
        exit(1);
    } else if (axis == 2) {
        slice = CreateMatrix(dictionary->shape[1], dictionary->shape[0]);

        for (int r = 0; r < slice.rows; r++) {
            for (int c = 0; c < slice.cols; c++) {
                slice.array[r][c] = dictionary->data[c][r][index];
            }
        }
    } else {
        fprintf(stderr, "GetSpectrogramFromDictionary: axis must be 0, 1 or 2\n");
        exit(1);
    }


    return slice;
}

void NormaliseDictionary(Dictionary *dictionary) {
    double max = 0;

    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                if (dictionary->data[i][j][k] > max) {
                    max = dictionary->data[i][j][k];
                }
            }
        }
    }

    for (int i = 0; i < dictionary->shape[0]; i++) {
        for (int j = 0; j < dictionary->shape[1]; j++) {
            for (int k = 0; k < dictionary->shape[2]; k++) {
                dictionary->data[i][j][k] /= max;
            }
        }
    }
}

Dictionary GetDictionary(const char *piano_name) {

    Dictionary dictionary;

    // Iterate through piano_W
    char W_persisted_name[256]; // Adjust the size as needed

    // Create the formatted string
    snprintf(W_persisted_name, sizeof(W_persisted_name),
             "%sconv_dict_piano_%s.h5",
             "C:\\Users\\ruanb\\OneDrive\\Desktop\\Piano Transcripton\\Piano Transcription (C)\\data_persisted\\dictionaries\\",
             piano_name);

    LoadDictionary(&dictionary, W_persisted_name);

    return dictionary;
}


Dictionary LoadTranscriptionDictionary()
{
    Dictionary dict = GetDictionary("AkPnBcht");

    Dictionary filtered = HardFilterSpectrograms(&dict, 1500);

    DestroyDictionary(&dict);

    NormaliseDictionary(&filtered);

    return filtered;
}

void DestroyDictionary(Dictionary *dictionary)
{
    for(int i = 0; i < dictionary->shape[0]; i++)
    {
        for(int j = 0; j < dictionary->shape[1]; j++)
        {
            free(dictionary->data[i][j]);
        }
        free(dictionary->data[i]);
    }
}

