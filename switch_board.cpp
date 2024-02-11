#include "switch_board.h"

std::array<daisy::Pin, SwitchRowCount> output_pin_configuration = {
    daisy::seed::D10, daisy::seed::D11, daisy::seed::D12,
    daisy::seed::D13, daisy::seed::D14, daisy::seed::D17
};


SwitchBoard::SwitchBoard() {
    m_clk.Init(daisy::seed::D8, daisy::GPIO::Mode::OUTPUT);
	m_ce.Init(daisy::seed::D9, daisy::GPIO::Mode::OUTPUT);
	m_pl.Init(daisy::seed::D7, daisy::GPIO::Mode::OUTPUT);
	//switch_out.Init(daisy::seed::D10, daisy::GPIO::Mode::INPUT);

    for(size_t i = 0; i < SwitchRowCount; ++i) {
        m_shr_outputs[i].Init(output_pin_configuration[i], daisy::GPIO::Mode::INPUT);
    }
}

void SwitchBoard::update() {
    // CP and CE should be low before the low-to-high transition
    // of PL to prevent shifting the data when PL goes high.
    m_clk.Write(false);
	m_ce.Write(false);

    // Parallel load data
    m_pl.Write(false);
	m_pl.Write(true);


    // TODO: Perhaps remove m_current_state and return a "context" struct that contains the state and the flow direction?
    for(int column = 0; column < SwitchColumnCount; ++column) {


        for(int row = 0; row < SwitchRowCount; ++row) {
            m_current_state[row][column] = m_shr_outputs[row].Read();
        }

        m_clk.Write(true);
		m_clk.Write(false);
        //switches[i] = switch_out.Read();

    }

}

bool SwitchBoard::get_switch_state(size_t column, size_t row) {
    return m_current_state[row][column];
}