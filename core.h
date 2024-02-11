#ifndef DRUMS_CORE_H_
#define DRUMS_CORE_H_

#include "fatfs.h"
#include "led_array.h"

void halt_error(LedArray& leds, const char *msg);
void halt_on_fs_error(LedArray& leds, const char *context, FRESULT res);

#endif