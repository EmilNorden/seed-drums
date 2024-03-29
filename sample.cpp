#include "sample.h"
#include "daisy.h"
#include <algorithm>

float DSY_SDRAM_BSS sample_memory[3000000];

SamplePlayback::SamplePlayback()
    : SamplePlayback(nullptr, nullptr, 0) {
}

SamplePlayback::SamplePlayback(float *base, float *sample, size_t length)
    : m_base_ptr(base), m_sample_ptr(sample), m_length(length) {
}

float SamplePlayback::sample() {
    float sample = *m_sample_ptr;
    m_sample_ptr++;

    return sample;
}

bool SamplePlayback::has_data() { return m_sample_ptr != nullptr; }

bool SamplePlayback::finished() {
    return m_sample_ptr == m_base_ptr + m_length;
}

void SamplePlayback::restart() {
    m_sample_ptr = m_base_ptr;
}

Sample::Sample()
    : m_data(nullptr), m_length(0) {
}

SamplePlayback Sample::into_playback() {
    SamplePlayback playback(
        m_data,
        m_data,
        m_length
    );

    return playback;
}


void SampleCollection::add(size_t sample_index, float *data, size_t length) {
    if(!m_samples[sample_index].has_data()) {
        if(sample_index == 0) {
            m_samples[sample_index].set_data(sample_memory);
        }
        else {
            auto& prev_sample = m_samples[sample_index - 1];
            m_samples[sample_index].set_data(prev_sample.data() + prev_sample.length());
        }
    }

    std::copy(
        data,
        data+length,
        m_samples[sample_index].data() + m_samples[sample_index].length());

    m_samples[sample_index].set_length(m_samples[sample_index].length() + length);
}

SamplePlayback SampleCollection::get(size_t sample_index) {
    return m_samples[sample_index].into_playback();
}

SampleBuffer::SampleBuffer()
    : m_next_sample_index(0) {

    }

float SampleBuffer::sample() {
    float result = 0.0f;
    for(auto &s : m_playing_samples) {
        if(s.has_data() && !s.finished()) {
            result += s.sample();
        }
    }
    return result;
}

void SampleBuffer::play(SamplePlayback sample) {
    m_playing_samples[m_next_sample_index] = sample;

    m_next_sample_index =
        (m_next_sample_index + 1) % NumberOfConcurrentSamples;
}