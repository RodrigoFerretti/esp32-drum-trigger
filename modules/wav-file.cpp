#include "Arduino.h"
#include <../modules/sd-memory.cpp>

const int WAV_HEADER_SIZE = 44;

struct wav_header_t
{
    char riff[4];
    uint32_t size;
    char wave[4];
    char fmt[4];
    uint32_t rest_size;
    uint16_t format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data[4];
    uint32_t data_size;
    uint32_t extra_size;
};

struct wav_file_t
{
    File file;
    wav_header_t header;
    int sample_bytes_size;

    void setup(const char *path)
    {
        file = SD.open(path);
        file.read((byte *)&header, WAV_HEADER_SIZE);
        sample_bytes_size = header.bits_per_sample / 8;
        int bytes_seek = WAV_HEADER_SIZE;
        header.extra_size = 0;
        while ((header.data[0] != *"d") & (header.data[1] != *"a") & (header.data[2] != *"t") & (header.data[3] != *"a"))
        {
            header.extra_size += header.data_size;
            file.seek(bytes_seek + header.extra_size);
            file.read((byte *)&header.data, 4);
            file.read((byte *)&header.data_size, 4);
            bytes_seek += 8;
        }
    }
};
