#ifndef DRUMS_LED_ARRAY_H_
#define DRUMS_LED_ARRAY_H_

#include "daisy_seed.h"

class LedArray {
public:
    LedArray();

    void set(uint8_t value);
private:
    daisy::GPIO m_clock;
	daisy::GPIO m_ser;
	daisy::GPIO m_latch;
	daisy::GPIO m_output_enable;
	daisy::GPIO m_clear;

    void toggle_clock();
    void toggle_latch();
};

#endif