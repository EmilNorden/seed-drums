#ifndef DRUMS_SAMPLES_H_
#define DRUMS_SAMPLES_H_

#include <cstdint>
#include <array>

const int NumberOfSamples = 8;

class SamplePlayback {
    public:
    SamplePlayback();
    SamplePlayback(int16_t *base, int16_t *sample, size_t length);

    int16_t sample();
    bool finished();
    void restart();
    private:

    int16_t *m_base_ptr;
    int16_t *m_sample_ptr;
    size_t m_length;
};

class Sample {
public:
    Sample();
    int16_t *data() { return m_data; }
    bool has_data() { return m_data != nullptr; }
    void set_data(int16_t *data) { m_data = data; }
    size_t length() { return m_length; }
    void set_length(size_t length) { m_length = length; }
    SamplePlayback into_playback();
private:
    int16_t *m_data;
    size_t m_length;
};

class SampleCollection {
public:
    void add(size_t sample_index, int16_t *data, size_t length);
    SamplePlayback get(size_t sample_index);
private:
    std::array<Sample, NumberOfSamples> m_samples;
};

#endif