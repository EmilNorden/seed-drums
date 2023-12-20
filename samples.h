#ifndef DRUMS_SAMPLES_H_
#define DRUMS_SAMPLES_H_

#include <cstdint>

struct SamplePlayback {
    int16_t *base_ptr;
    int16_t *sample_ptr;
    size_t length;
};

void samples_init();

void samples_add(size_t sample_index, int16_t *data, size_t length);

SamplePlayback samples_get(size_t sample_index);

int16_t samples_sample(SamplePlayback& playback);

#endif