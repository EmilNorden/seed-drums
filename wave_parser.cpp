#include "wave_parser.h"
#include "sample.h"
#include "core.h"
#include "fatfs.h"
#include <cstdio>
#include <algorithm>
#include <limits>
const size_t read_buf_size = 4096;

/*
Due to DMA limitations for the SDMMC peripheral, it can only access data in the D1 RAM section.
This means that these variables needs to live in global memory or be dynamically allocated. It cannot live on the stack.
*/
FIL current_file;
int16_t read_buffer[read_buf_size];

WaveResult wave_load(size_t sample_number, SampleCollection &samples) {
    
    char path[64];
    sprintf(path, "/%d.wav", sample_number);
    FRESULT fres;
    fres = f_open(&current_file, path, (FA_OPEN_EXISTING | FA_READ));
    halt_on_fs_error("wave open", fres);

    size_t bytes_read;
    WaveHeader header;
    fres = f_read(&current_file, (void*)&header, sizeof(WaveHeader), &bytes_read);
    halt_on_fs_error("wave read1", fres);
    if(bytes_read != sizeof(WaveHeader)) {
        char str[32];
        sprintf(str, "header fail - %u", bytes_read);
        halt_error(str);
        return WaveResult::InvalidHeader;
    }

    //char str[16];
    //sprintf(str, "-> %u", header.data_id);
    // header.file_id = 0x46464952 (1179011410)
    // header.wave_id = 0x45564157
    // header.data_id = 0x61746164;


    //halt_error(str);


    
    int i = 0;
    while(true) {
        i++;
        bytes_read = 0;
        fres = f_read(
            &current_file,
            (void*)read_buffer,
            read_buf_size,
            &bytes_read);

        halt_on_fs_error("wave rloop", fres);

        auto number_of_samples = bytes_read / sizeof(read_buffer[0]);
        float sample_array[read_buf_size];

        // TODO: This assumes mono channel 16-bit PCM. Perhaps write handlers for more formats?
        std::transform(
            read_buffer,
            read_buffer + number_of_samples,
            sample_array,
            [] (int16_t sample) { 
                return sample / static_cast<float>(std::numeric_limits<int16_t>::max()); 
            }
        );
        samples.add(sample_number, sample_array, number_of_samples);

        if(bytes_read < read_buf_size || f_eof(&current_file)) {
            break;
        }
    }
    
    f_close(&current_file);

    return WaveResult::OK;
}