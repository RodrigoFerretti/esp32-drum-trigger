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

    void setup(File _file)
    {
        file = _file;
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

    void print_wav_data()
    {
        Serial.print("riff: ");
        Serial.print(header.riff[0]);
        Serial.print(header.riff[1]);
        Serial.print(header.riff[2]);
        Serial.println(header.riff[3]);

        Serial.print("fmt: ");
        Serial.print(header.fmt[0]);
        Serial.print(header.fmt[1]);
        Serial.print(header.fmt[2]);
        Serial.println(header.fmt[3]);

        Serial.print("size: ");
        Serial.println(header.size);

        Serial.print("wave: ");
        Serial.print(header.wave[0]);
        Serial.print(header.wave[1]);
        Serial.print(header.wave[2]);
        Serial.println(header.wave[3]);

        Serial.print("fmt: ");
        Serial.print(header.fmt[0]);
        Serial.print(header.fmt[1]);
        Serial.print(header.fmt[2]);
        Serial.println(header.fmt[3]);

        Serial.print("rest_size: ");
        Serial.println(header.rest_size);

        Serial.print("format: ");
        Serial.println(header.format);

        Serial.print("channels: ");
        Serial.println(header.channels);

        Serial.print("sample_rate: ");
        Serial.println(header.sample_rate);

        Serial.print("byte_rate: ");
        Serial.println(header.byte_rate);

        Serial.print("block_align: ");
        Serial.println(header.block_align);

        Serial.print("bits_per_sample: ");
        Serial.println(header.bits_per_sample);

        Serial.print("data: ");
        Serial.print(header.data[0]);
        Serial.print(header.data[1]);
        Serial.print(header.data[2]);
        Serial.println(header.data[3]);

        Serial.print("data_size: ");
        Serial.println(header.data_size);
    }
};
