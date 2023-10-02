//
// Created by ruanb on 9/10/2023.
//

#include "wav.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

// WAVE header structure

unsigned char buffer4[4];
unsigned char buffer2[2];

char *seconds_to_time(float seconds);


//


WavFile ReadWav(const char *filename) {

    printf("Opening file %s\n", filename);

    FILE *ptr;

    ptr = fopen(filename, "rb");
    if (ptr == NULL) {
        printf("Error opening file\n");
        printf(filename);
        exit(1);
    }


    printf("Reading header..\n");

    WavHeader header = ReadWavHeader(ptr);

    printf("Header read..\n");

    printf("Reading data..\n");


    // calculate no.of samples
    header.num_samples = (8 * header.data_size) / (header.channels * header.bits_per_sample);
//    printf("Number of samples:%lu \n", num_samples);

//    DynamicArray wav_buffer = CreateDynamicArray(num_samples);

    WavFile wav_buffer = CreateWavFile(header.num_samples, header);
//    WavFile wav_buffer = CreateWavFile(header.num_samples / header.channels, header);

    unsigned int size_of_each_sample = (header.channels * header.bits_per_sample) / 8;
//    printf("Size of each sample:%ld bytes\n", size_of_each_sample);

    // calculate duration of file
    float duration_in_seconds = (float) header.overall_size / header.byterate;
//    printf("Approx.Duration in seconds=%f\n", duration_in_seconds);
//    printf("Approx.Duration in h:m:s=%s\n", seconds_to_time(duration_in_seconds));

    // read each sample from data chunk if PCM
    if (header.format_type == 1) { // PCM
//        scanf("%c", &c);
        long i = 0;
        char data_buffer[size_of_each_sample];
        int size_is_correct = TRUE;

        // make sure that the bytes-per-sample is completely divisible by num.of channels
        unsigned long bytes_in_each_channel = (size_of_each_sample / header.channels);
        if ((bytes_in_each_channel * header.channels) != size_of_each_sample) {
            printf("Error: %ld x %ud <> %ud\n", bytes_in_each_channel, header.channels, size_of_each_sample);
            size_is_correct = FALSE;
        }

        if (size_is_correct) {
            // the valid amplitude range for values based on the bits per sample
            long low_limit = 0l;
            long high_limit = 0l;

            switch (header.bits_per_sample) {
                case 8:
                    low_limit = -128;
                    high_limit = 127;
                    break;
                case 16:
                    low_limit = -32768;
                    high_limit = 32767;
                    break;
                case 32:
                    low_limit = -2147483648;
                    high_limit = 2147483647;
                    break;
            }

//                printf("\n\nValid range for data values : %ld to %ld \n", low_limit, high_limit);
            for (i = 1; i <= header.num_samples; i++) {
//                printf("==========Sample %ld / %ld=============\n", i, header.num_samples);
                unsigned long read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
                if (read == 1) {

                    // dump the data read
                    unsigned int xchannels = 0;
                    int data_in_channel = 0;
                    int offset = 0; // move the offset for every iteration in the loop below
                    for (xchannels = 0; xchannels < header.channels; xchannels++) {
//                        printf("Channel#%d : ", (xchannels + 1));
                        // convert data from little endian to big endian based on bytes in each channel sample
                        if (bytes_in_each_channel == 4) {
                            data_in_channel = (data_buffer[offset] & 0x00ff) |
                                              ((data_buffer[offset + 1] & 0x00ff) << 8) |
                                              ((data_buffer[offset + 2] & 0x00ff) << 16) |
                                              (data_buffer[offset + 3] << 24);
                        } else if (bytes_in_each_channel == 2) {
                            data_in_channel = (data_buffer[offset] & 0x00ff) |
                                              (data_buffer[offset + 1] << 8);
                        } else if (bytes_in_each_channel == 1) {
                            data_in_channel = data_buffer[offset] & 0x00ff;
                            data_in_channel -= 128; //in wave, 8-bit are unsigned, so shifting to signed
                        }

                        offset += bytes_in_each_channel;
//                        printf("%d ", data_in_channel);

                        // check if value was in range
                        if (data_in_channel < low_limit || data_in_channel > high_limit)
                            fprintf(stderr, "Reading wav: value out of range\n");

//
//                        wav_buffer.channels[xchannels].array[i/(header.channels+1) + xchannels] = data_in_channel;
                        wav_buffer.channels[xchannels].array[i] = (double)data_in_channel/high_limit;
                    }

//                        printf("\n");
                } else {
                    printf("Error reading file. %ld bytes\n", read);
                    break;
                }

            } // 	for (i =1; i <= num_samples; i++) {

        } // 	if (size_is_correct) {

        // if (c == 'Y' || c == 'y') {
    } //  if (header.format_type == 1) {

    printf("Closing file..\n");
    fclose(ptr);
    printf("File closed\n");

    return wav_buffer;
}

WavFile CreateWavFile(unsigned long numSamples, WavHeader headersss) {
    WavFile result;

    result.header = headersss;
    result.channels[0] = CreateDynamicArray(numSamples);
    result.channels[1] = CreateDynamicArray(numSamples);

    return result;
}

WavHeader ReadWavHeader(FILE *ptr) {
    unsigned long read = 0;

    // read header parts

    WavHeader header;

    read = fread(header.riff, sizeof(header.riff), 1, ptr);
//    printf("(1-4): %s \n", header.riff);

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
//    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    // convert little endian to big endian 4 byte int
    header.overall_size = buffer4[0] |
                          (buffer4[1] << 8) |
                          (buffer4[2] << 16) |
                          (buffer4[3] << 24);

//    printf("(5-8) Overall size: bytes:%u, Kb:%u \n", header.overall_size, header.overall_size / 1024);

    read = fread(header.wave, sizeof(header.wave), 1, ptr);
//    printf("(9-12) Wave marker: %s\n", header.wave);

    read = fread(header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), 1, ptr);
//    printf("(13-16) Fmt marker: %s\n", header.fmt_chunk_marker);

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
//    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    // convert little endian to big endian 4 byte integer
    header.length_of_fmt = buffer4[0] |
                           (buffer4[1] << 8) |
                           (buffer4[2] << 16) |
                           (buffer4[3] << 24);
//    printf("(17-20) Length of Fmt header: %u \n", header.length_of_fmt);

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
//    printf("%u %u \n", buffer2[0], buffer2[1]);

    header.format_type = buffer2[0] | (buffer2[1] << 8);
    char format_name[10] = "";
    if (header.format_type == 1)
        strcpy(format_name, "PCM");
    else if (header.format_type == 6)
        strcpy(format_name, "A-law");
    else if (header.format_type == 7)
        strcpy(format_name, "Mu-law");

//    printf("(21-22) Format type: %u %s \n", header.format_type, format_name);

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
//    printf("%u %u \n", buffer2[0], buffer2[1]);

    header.channels = buffer2[0] | (buffer2[1] << 8);
//    printf("(23-24) Channels: %u \n", header.channels);

    if (header.channels > 2) {
        fprintf(stderr, "Error: more than 2 channels is not supported\n");
        exit(1);
    }

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
//    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    header.sample_rate = buffer4[0] |
                         (buffer4[1] << 8) |
                         (buffer4[2] << 16) |
                         (buffer4[3] << 24);

//    printf("(25-28) Sample rate: %u\n", header.sample_rate);

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
//    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    header.byterate = buffer4[0] |
                      (buffer4[1] << 8) |
                      (buffer4[2] << 16) |
                      (buffer4[3] << 24);
//    printf("(29-32) Byte Rate: %u, Bit Rate:%u\n", header.byterate, header.byterate * 8);

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
//    printf("%u %u \n", buffer2[0], buffer2[1]);

    header.block_align = buffer2[0] |
                         (buffer2[1] << 8);
//    printf("(33-34) Block Alignment: %u \n", header.block_align);

    read = fread(buffer2, sizeof(buffer2), 1, ptr);
//    printf("%u %u \n", buffer2[0], buffer2[1]);

    header.bits_per_sample = buffer2[0] |
                             (buffer2[1] << 8);
//    printf("(35-36) Bits per sample: %u \n", header.bits_per_sample);

    read = fread(header.data_chunk_header, sizeof(header.data_chunk_header), 1, ptr);
//    printf("(37-40) Data Marker: %s \n", header.data_chunk_header);

    read = fread(buffer4, sizeof(buffer4), 1, ptr);
//    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

    header.data_size = buffer4[0] |
                       (buffer4[1] << 8) |
                       (buffer4[2] << 16) |
                       (buffer4[3] << 24);
//    printf("(41-44) Size of data chunk: %u \n", header.data_size);

    return header;
}

void NormaliseWav(WavFile *wavFile) {
    NormaliseChannel(&wavFile->channels[0]);
    NormaliseChannel(&wavFile->channels[1]);
}

void TestWav() {
    char *wav_file_path = "../tests/test.wav";


    WavFile wavdata = ReadWav(wav_file_path);

    // Print wavdata
    for (int i = 0; i < wavdata.channels[0].size; i++) {
        printf("%f\n", wavdata.channels[0].array[i]);
    }
}

DynamicArray StereoToMono(WavFile *wavFile, const char *process) {
    DynamicArray mono = CreateDynamicArray(wavFile->channels[0].size);

    for(int i = 0;i < mono.size; i++){
        if (strcmp(process, "sum") == 0){
            mono.array[i] = wavFile->channels[0].array[i] + wavFile->channels[1].array[i];
        }else if(strcmp(process, "average") == 0){
            mono.array[i] = (wavFile->channels[0].array[i] + wavFile->channels[1].array[i])/2;
//            mono.array[i] = (wavFile->channels[0].array[i] + wavFile->channels[1].array[i])/4;
        } else{
            mono.array[i] = wavFile->channels[atoi(process)].array[i];
        }
    }

    return mono;
}

void NormaliseChannel(DynamicArray *wavdata) {
    double max = 0;
    for (int i = 0; i < wavdata->size; i++) {
        if (wavdata->array[i] > max) {
            max = wavdata->array[i];
        }
    }

    for (int i = 0; i < wavdata->size; i++) {
        wavdata->array[i] /= max;
    }
}

/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *	seconds - seconds value
 * Returns: hms - formatted string
 **/
char *seconds_to_time(float raw_seconds) {
    char *hms;
    int hours, hours_residue, minutes, seconds, milliseconds;
    hms = (char *) malloc(100);

    sprintf(hms, "%f", raw_seconds);

    hours = (int) raw_seconds / 3600;
    hours_residue = (int) raw_seconds % 3600;
    minutes = hours_residue / 60;
    seconds = hours_residue % 60;
    milliseconds = 0;

    // get the decimal part of raw_seconds to get milliseconds
    char *pos;
    pos = strchr(hms, '.');
    int ipos = (int) (pos - hms);
    char decimalpart[15];
    memset(decimalpart, ' ', sizeof(decimalpart));
    strncpy(decimalpart, &hms[ipos + 1], 3);
    milliseconds = atoi(decimalpart);


    sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
    return hms;
}
