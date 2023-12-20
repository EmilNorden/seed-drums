#ifndef DRUMS_SAMPLES_H_
#define DRUMS_SAMPLES_H_

#include <cstdint>
#include <array>

const int NumberOfSamples = 8;

class SamplePlayback {
    public:
    SamplePlayback();
    SamplePlayback(float *base, float *sample, size_t length);

    float sample();
    bool finished();
    void restart();
    private:

    float *m_base_ptr;
    float *m_sample_ptr;
    size_t m_length;
};

class Sample {
public:
    Sample();
    float *data() { return m_data; }
    bool has_data() { return m_data != nullptr; }
    void set_data(float *data) { m_data = data; }
    size_t length() { return m_length; }
    void set_length(size_t length) { m_length = length; }
    SamplePlayback into_playback();
private:
    float *m_data;
    size_t m_length;
};

class SampleCollection {
public:
    void add(size_t sample_index, float *data, size_t length);
    SamplePlayback get(size_t sample_index);
private:
    std::array<Sample, NumberOfSamples> m_samples;
};

#endif