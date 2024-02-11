#ifndef DRUMS_WAVE_PARSER_H_
#define DRUMS_WAVE_PARSER_H_

#include <cstdint>
#include "sample.h"
#include "led_array.h"

struct WaveHeader {
    uint32_t file_id;
    uint32_t file_size;
    uint32_t wave_id;
    uint32_t fmt_id;
    uint32_t fmt_chunk_size;
    uint16_t format;
    uint16_t number_of_channels;
    uint32_t sample_freq;
    uint32_t bytes_per_sec;
    uint16_t block_alignment;
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_size;
};

enum class WaveResult {
    OK,
    OpenFailure,
    InvalidHeader,
    ReadFailure,
};

WaveResult wave_load(LedArray &leds, size_t sample_number, SampleCollection &samples);

#endif