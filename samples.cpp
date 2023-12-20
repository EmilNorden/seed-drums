#include "samples.h"
#include "daisy.h"
#include <algorithm>



int16_t DSY_SDRAM_BSS sample_memory[3000000];

struct Sample {
    int16_t *base_ptr;
    size_t length;
};

struct SampleTable {
    Sample samples[8];
};

SampleTable sample_table;

void samples_init() {
    for(int i = 0; i < 8; ++i) {
        sample_table.samples[i].base_ptr = nullptr;
        sample_table.samples[i].length = 0;
    }
    
}


void samples_add(size_t sample_index, int16_t *data, size_t length) {
    
    if(sample_table.samples[sample_index].base_ptr == nullptr) {
        if(sample_index == 0) {
            sample_table.samples[sample_index].base_ptr = sample_memory;
        }
        else {
            sample_table.samples[sample_index].base_ptr = 
                sample_table.samples[sample_index - 1].base_ptr +
                sample_table.samples[sample_index - 1].length;
        }
    }

    std::copy(
        data,
        data+length,
        sample_table.samples[sample_index].base_ptr + sample_table.samples[sample_index].length);

    sample_table.samples[sample_index].length += length;
}

SamplePlayback samples_get(size_t sample_index) {
    SamplePlayback playback;
    playback.base_ptr = sample_table.samples[sample_index].base_ptr;
    playback.sample_ptr = sample_table.samples[sample_index].base_ptr;
    playback.length = sample_table.samples[sample_index].length;

    return playback;
}

int16_t samples_sample(SamplePlayback& playback) {
    int16_t sample = *playback.sample_ptr;
    playback.sample_ptr++;
    if(playback.sample_ptr == playback.base_ptr + playback.length) {
        playback.sample_ptr = playback.base_ptr;
    }

    return sample;
}