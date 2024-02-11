#include "led_array.h"

LedArray::LedArray() {
    m_clock.Init(daisy::seed::D27, daisy::GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	m_ser.Init(daisy::seed::D24,daisy::GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	m_latch.Init(daisy::seed::D26, daisy::GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	m_output_enable.Init(daisy::seed::D25, daisy::GPIO::Mode::OUTPUT);
	m_clear.Init(daisy::seed::D28, daisy::GPIO::Mode::OUTPUT);

	m_output_enable.Write(false);
	m_clear.Write(true);
}

void LedArray::set(uint8_t value) {
    for(int i = 0; i < 8; i++){
        //output.Write(true);
        if(value & (1 << i)) {
            m_ser.Write(true);
        }
        else {
            m_ser.Write(false);
        }
        //daisy::System::Delay(40);
        toggle_clock();
    }
    toggle_latch();
}

void LedArray::toggle_clock() {
	m_clock.Write(true);
	//daisy::System::Delay(2);
	m_clock.Write(false);
	//daisy::System::Delay(2);
}

void LedArray::toggle_latch() {
	m_latch.Write(true);
	//daisy::System::Delay(4);
	m_latch.Write(false);
	//daisy::System::Delay(4);
}