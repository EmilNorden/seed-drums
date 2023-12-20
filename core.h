#ifndef DRUMS_CORE_H_
#define DRUMS_CORE_H_

#include "fatfs.h"

void halt_error(const char *msg);
void halt_on_fs_error(const char *context, FRESULT res);

#endif