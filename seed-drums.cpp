#include "daisy_seed.h"
#include "daisysp.h"
#include "sample.h"
#include "wave_parser.h"
#include "switch_board.h"
#include <system.h>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
SdmmcHandler   sdcard;
FatFSInterface fsi;
SampleBuffer sample_buffer;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{

	for (size_t i = 0; i < size; i++)
	{

		//auto sample = machine.sample();
		auto sample = sample_buffer.sample();
		float volume = 5.0f;
		out[0][i] = (sample) * volume;
		out[1][i] = (sample) * volume;
	}
}

void halt_error(const char *msg) {
		while(1) {
			hw.PrintLine(msg);
		}
}

void halt_on_fs_error(const char *context, FRESULT res) {
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
		
	halt_error(message);
}

void set_leds(GPIO &latch, GPIO &clock, GPIO &output, bool stats[8]) {
	latch.Write(false);
	//clock.Write(false);
	for(int i = 0; i < 8; ++i) {
		clock.Write(false);
		//output.Write((value & (1 << i));)
		/*if(f & 0x1) {
			output.Write(true);
		}
		else {
			output.Write(false);
		}*/
		output.Write(stats[i]);
		clock.Write(true);
		//daisy::System::DelayUs(1);
		//hw.DelayUs(1);
		
		//f = f >> 1;
	}
	latch.Write(true);
}

/*void set_leds(GPIO &latch, GPIO &clock, GPIO &output, uint8_t leds) {
	latch.Write(false);
	//clock.Write(false);
	for(int i = 0; i < 8; ++i) {
		if(leds & 0x1) {
			output.Write(true);
		}
		else {
			output.Write(false);
		}
		
		clock.Write(true);
		daisy::System::DelayUs(1);
		//hw.DelayUs(1);
		leds = leds >> 1;
		latch.Write(true);
	}
}*/


int main(void)
{
	size_t blocksize = 128;
	hw.Init();
	hw.StartLog(false);

	GPIO clock;
	GPIO ser;
	GPIO latch;

	clock.Init(daisy::seed::D14, GPIO::Mode::OUTPUT);
	ser.Init(daisy::seed::D13, GPIO::Mode::OUTPUT);
	latch.Init(daisy::seed::D12, GPIO::Mode::OUTPUT);

	SwitchBoard switches;

	SdmmcHandler::Config sd_cfg;
	//sd_cfg.speed = SdmmcHandler::Speed::SLOW;
	//sd_cfg.width =  SdmmcHandler::BusWidth::BITS_1;
	sd_cfg.Defaults();
	sd_cfg.speed = SdmmcHandler::Speed::SLOW;
    if(sdcard.Init(sd_cfg) == SdmmcHandler::Result::ERROR) {
		halt_error("SDMMC ERROR");
	}
	
	auto fsi_result = fsi.Init(FatFSInterface::Config::MEDIA_SD);
	if(fsi_result == FatFSInterface::Result::ERR_TOO_MANY_VOLUMES) {
		halt_error("ERR_TOO_MANY_VOLUMES");
	} else if(fsi_result == FatFSInterface::Result::ERR_NO_MEDIA_SELECTED) {
		halt_error("ERR_NO_MEDIA_SELECTED");
	} else if(fsi_result == FatFSInterface::Result::ERR_GENERIC) {
		halt_error("ERR_GENERIC");
	}

	FRESULT fres = f_mount(&fsi.GetSDFileSystem(), "/", 1);
	halt_on_fs_error("f_mount", fres);

	SampleCollection samples;
	//samples_init();

	for(int i = 0; i < 8; ++i) {
	WaveResult res = wave_load(i, samples);
	if(res == WaveResult::OpenFailure) {
		halt_error("WAVE FAIL - open");
	} else if(res == WaveResult::InvalidHeader) {
		halt_error("WAVE FAIL - header");
	} else if(res == WaveResult::ReadFailure) {
		halt_error("WAVE FAIL - read");
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


	GPIO switch_clock;
	GPIO switch_ce;
	GPIO switch_pl;
	GPIO switch_out;

	/*switch_clock.Init(daisy::seed::D8, GPIO::Mode::OUTPUT);
	switch_ce.Init(daisy::seed::D9, GPIO::Mode::OUTPUT);
	switch_pl.Init(daisy::seed::D7, GPIO::Mode::OUTPUT);
	switch_out.Init(daisy::seed::D10, GPIO::Mode::INPUT);*/

	int current_column = 0;
	int step_dir = 1;
	
	while(1) {

		// Parallel load
		/*switch_clock.Write(false);
		switch_ce.Write(false);

		switch_pl.Write(false);
		switch_pl.Write(true);

		for(int i = 0; i < 8; ++i) {
			switches[i] = switch_out.Read();
			switch_clock.Write(true);
			switch_clock.Write(false);
		}*/

		/*hw.PrintLine("%d %d %d %d %d %d %d %d",
			switches[0],
			switches[1],
			switches[2],
			switches[3],
			switches[4],
			switches[5],
			switches[6],
			switches[7]);
		*/

		uint32_t new_tick = timer.GetTick();
		float intervalMsec = 1000. * ((float)(new_tick - last_tick) / (float)freq);
		if(intervalMsec >= 500) {
			last_tick = new_tick;
			switches.update();
			current_column = current_column + step_dir;
			if(current_column >= SwitchColumnCount) {
				current_column = 0;
			} else if(current_column < 0) {
				current_column = SwitchColumnCount - 1;
			}

			for(int row = 0; row < SwitchRowCount; ++row) {
				if(switches.get_switch_state(current_column, row)) {
					sample_buffer.play(samples.get(row));
				}
			}
			/*set_leds(latch, clock, ser, leds);
			counter++;
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
