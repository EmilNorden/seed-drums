#include "daisy_seed.h"
#include "daisysp.h"
#include "samples.h"
#include "wave_parser.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
SdmmcHandler   sdcard;
FatFSInterface fsi;

static int v = 0;
bool b = true;
SamplePlayback playback;

// Samples 112404
// 56202 but stereo
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{

	for (size_t i = 0; i < size; i++)
	{

		auto sample = samples_sample(playback);
		float volume = 10.0f;
		out[0][i] = (sample / 32768.0f) * volume;
		out[1][i] = (sample / 32768.0f) * volume;
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


void load_wave() {
	
}

int main(void)
{
	size_t blocksize = 128;
	hw.Init();
	hw.StartLog(false);

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

	samples_init();

size_t samplenr = 0;
WaveResult res = wave_load(0);
	res = wave_load(1);
	if(res == WaveResult::OpenFailure) {
		halt_error("WAVE FAIL - open");
	} else if(res == WaveResult::InvalidHeader) {
		halt_error("WAVE FAIL - header");
	} else if(res == WaveResult::ReadFailure) {
		halt_error("WAVE FAIL - read");
	}


	playback = samples_get(samplenr);

	hw.SetAudioBlockSize(blocksize); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	//osc.Init(hw.AudioSampleRate());
	//osc.SetFreq(440);
	hw.StartAudio(AudioCallback);
	unsigned int i = 0;
	while(1) {
		//sampler.Prepare();
	
	}
}
