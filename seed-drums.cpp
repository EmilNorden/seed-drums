#include "daisy_seed.h"
#include "daisysp.h"
#include "sample.h"
#include "wave_parser.h"
#include "switch_board.h"
#include "led_array.h"
#include <system.h>
#include <cmath>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
SdmmcHandler   sdcard;
FatFSInterface fsi;
SampleBuffer sample_buffer;
 
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float volume = (1.0f - hw.adc.GetFloat(1)) * 5.0f;
	//float volume = 1.5f;
	for (size_t i = 0; i < size; i++)
	{
		//auto sample = machine.sample();
		auto sample = sample_buffer.sample();
		out[0][i] = (sample) * volume;
		out[1][i] = (sample) * volume;
	}
}

void halt_error(LedArray &leds, const char *msg) {
		uint8_t led_mask = 85;
		while(1) {
			hw.PrintLine(msg);
			leds.set(led_mask);
			led_mask = led_mask ^ 0xFF;
			daisy::System::Delay(500);
		}
}

void halt_on_fs_error(LedArray &leds, const char *context, FRESULT res) {
	if(res == FR_OK) {
		return;
	}

	char message[64];
	switch(res) {
		case FR_INVALID_DRIVE:
		sprintf(message, "%s %s", context, "FR_INVALID_DRIVE");
		break;
		case FR_DISK_ERR:
		sprintf(message, "%s %s", context, "FR_DISK_ERR");
		break;
		case FR_INT_ERR:
		sprintf(message, "%s %s", context, "FR_INT_ERR");
		break;
		case FR_NOT_READY:
		sprintf(message, "%s %s", context, "FR_NOT_READY");
		break;
		case FR_NOT_ENABLED:
		sprintf(message, "%s %s", context, "FR_NOT_ENABLED");
		break;
		case FR_NO_FILESYSTEM:
		sprintf(message, "%s %s", context, "FR_NO_FILESYSTEM");
		break;
		default:
		sprintf(message, "%s %s", context, "UNKNOWN ERROR");
	}
		
	halt_error(leds, message);
}

int main(void)
{
	size_t blocksize = 128;
	hw.Init();
	hw.StartLog(false);

	/*GPIO clock;
	GPIO ser;
	GPIO latch;
	GPIO output_enable;
	GPIO clear;*/

	/*clock.Init(daisy::seed::D27, GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	ser.Init(daisy::seed::D24, GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	latch.Init(daisy::seed::D26, GPIO::Mode::OUTPUT);//, GPIO::Pull::NOPULL, GPIO::Speed::HIGH);
	output_enable.Init(daisy::seed::D25, GPIO::Mode::OUTPUT);
	clear.Init(daisy::seed::D28, GPIO::Mode::OUTPUT);

	output_enable.Write(false);
	clear.Write(true);*/

	//set_leds(latch, clock, ser, 85);
	//set_leds(latch, clock, ser, 85);
	//halt_error("leds are set!");

	LedArray leds;

	SwitchBoard switches;

	SdmmcHandler::Config sd_cfg;
	//sd_cfg.speed = SdmmcHandler::Speed::SLOW;
	//sd_cfg.width =  SdmmcHandler::BusWidth::BITS_1;
	sd_cfg.Defaults();
	sd_cfg.speed = SdmmcHandler::Speed::SLOW;
    if(sdcard.Init(sd_cfg) == SdmmcHandler::Result::ERROR) {
		halt_error(leds, "SDMMC ERROR");
	}
	
	auto fsi_result = fsi.Init(FatFSInterface::Config::MEDIA_SD);
	if(fsi_result == FatFSInterface::Result::ERR_TOO_MANY_VOLUMES) {
		halt_error(leds, "ERR_TOO_MANY_VOLUMES");
	} else if(fsi_result == FatFSInterface::Result::ERR_NO_MEDIA_SELECTED) {
		halt_error(leds, "ERR_NO_MEDIA_SELECTED");
	} else if(fsi_result == FatFSInterface::Result::ERR_GENERIC) {
		halt_error(leds, "ERR_GENERIC");
	}

	FRESULT fres = f_mount(&fsi.GetSDFileSystem(), "/", 1);
	halt_on_fs_error(leds, "f_mount", fres);

	SampleCollection samples;
	//samples_init();

	for(int i = 0; i < 8; ++i) {
		WaveResult res = wave_load(leds, i, samples);
		if(res == WaveResult::OpenFailure) {
			halt_error(leds, "WAVE FAIL - open");
		} else if(res == WaveResult::InvalidHeader) {
			halt_error(leds, "WAVE FAIL - header");
		} else if(res == WaveResult::ReadFailure) {
			halt_error(leds, "WAVE FAIL - read");
		}
	}

	TimerHandle timer;
	timer.Init(TimerHandle::Config());
	timer.Start();

	hw.SetAudioBlockSize(blocksize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	//osc.Init(hw.AudioSampleRate());
	//osc.SetFreq(440);
	hw.StartAudio(AudioCallback);

	uint32_t freq = timer.GetFreq();
	uint32_t last_tick = timer.GetTick();
	//uint8_t leds = (1);


	int current_column = -1;
	int step_dir = 1;

	//GPIO tempo;
	//tempo.Init(daisy::seed::D15, GPIO::Mode::ANALOG);
	daisy::AdcChannelConfig adc_configs[2];
	adc_configs[0].InitSingle(daisy::seed::D15);
	adc_configs[1].InitSingle(daisy::seed::D16);
	hw.adc.Init(adc_configs, 2);
	hw.adc.Start();	

	constexpr size_t flow_control_row_index = 0;

	while(1) {
		const float MinDelay = 75;
		const float MaxDelay = 1000;
		float beat_delay = MinDelay + (hw.adc.GetFloat(0) * (MaxDelay - MinDelay));

		uint32_t new_tick = timer.GetTick();
		float intervalMsec = 1000. * ((float)(new_tick - last_tick) / (float)freq);
		if(intervalMsec >= beat_delay) {
			last_tick = new_tick;
			switches.update();

			current_column = current_column + step_dir;
			
			if(current_column >= SwitchColumnCount) {
				current_column = 0;
			} else if(current_column < 0) {
				current_column = SwitchColumnCount - 1;
			}
			leds.set(1 << (size_t)current_column);

			for(int row = 0; row < SwitchRowCount; ++row) {
				if(row == flow_control_row_index) {
					continue;
				}
				if(switches.get_switch_state(current_column, row)) {
					/*char msg[64];
					sprintf(msg, "here I am --> %d", current_column);
					halt_error(msg);*/
					sample_buffer.play(samples.get(row));
				}
			}

			//set_leds(latch, clock, ser, 1 << ((current_column+1) % SwitchColumnCount));
			//set_leds(latch, clock, ser,2);

			if(switches.get_switch_state(current_column, flow_control_row_index)) {
				step_dir = step_dir * -1;
			}

			
		
			
			/*counter++;
			if(counter == 2) {
				counter = 0;
				leds[active_led] = false;
				active_led = (active_led - 1);
				if(active_led < 0) {
					active_led = 7;
				}
				leds[active_led] = true;
			}*/
			// machine.step();
		}
	
	}
}
