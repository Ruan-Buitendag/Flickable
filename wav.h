//
// Created by ruanb on 9/10/2023.
//

#ifndef C_PIANO_TRANSCRIPTION_WAV_H
#define C_PIANO_TRANSCRIPTION_WAV_H

#include <dynamicarray.h>
#include "stdio.h"
#include "stdlib.h"

// WAVE file header format
typedef struct WavHeaderStruct{
    unsigned char riff[4];						// RIFF string
    unsigned int overall_size	;				// overall size of file in bytes
    unsigned char wave[4];						// WAVE string
    unsigned char fmt_chunk_marker[4];			// fmt string with trailing null char
    unsigned int length_of_fmt;					// length of the format data
    unsigned int format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
    unsigned int channels;						// no.of channels
    unsigned int sample_rate;					// sampling rate (blocks per second)
    unsigned int byterate;						// SampleRate * NumChannels * BitsPerSample/8
    unsigned int block_align;					// NumChannels * BitsPerSample/8
    unsigned int bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
    unsigned char data_chunk_header [4];		// DATA string or FLLR string
    unsigned int data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
    unsigned long num_samples;
} WavHeader;

typedef struct WavFileStruct{
    WavHeader header;
    DynamicArray channels[2];
} WavFile;

WavFile CreateWavFile(unsigned long numSamples, WavHeader header);

WavHeader ReadWavHeader(FILE* ptr);
WavFile ReadWav(const char *filename);
void NormaliseWav(WavFile *wavFile);
void NormaliseChannel(DynamicArray *wavdata);
DynamicArray StereoToMono(WavFile *wavFile, const char *process);

void TestWav();

#endif //C_PIANO_TRANSCRIPTION_WAV_H
