#ifndef DRUMS_SWITCH_HANDLER_H_
#define DRUMS_SWITCH_HANDLER_H_

#include "daisy_seed.h"
#include <array>

const size_t SwitchRowCount = 6;
const int SwitchColumnCount = 8;

class SwitchBoard {
    public:
    SwitchBoard();
    void update();

    bool get_switch_state(size_t column, size_t row);
    private:
    daisy::GPIO m_clk;
	daisy::GPIO m_ce;
	daisy::GPIO m_pl;

    std::array<daisy::GPIO, SwitchRowCount> m_shr_outputs;
    std::array<std::array<bool, SwitchColumnCount>, SwitchRowCount> m_current_state;

};

#endif