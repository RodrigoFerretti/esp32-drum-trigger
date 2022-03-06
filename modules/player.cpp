#include "Arduino.h"
#include <cstring>
#include <../modules/wav-file.cpp>

const int SAMPLE_AMMOUNT = 32;

struct player_t
{
    int16_t buffer[SAMPLE_AMMOUNT];
    int buffer_bytes_size;
    int buffer_samples_read;
    int file_bytes_read;
    bool is_playing;
    wav_file_t wavs[4];
    wav_file_t wav;

    void setup()
    {
        for (int i = 0; i < 4; i++)
        {
            wav_file_t wav_file;
            wavs[i] = wav_file;
            wavs[i].setup(sample_loader.get_file_path());
        }
    }

    void trigger_sample()
    {
        is_playing = true;
        wav = wavs[random(0, 4)];
        buffer_bytes_size = SAMPLE_AMMOUNT * wav.sample_bytes_size;
        wav.file.seek(WAV_HEADER_SIZE + wav.header.extra_size);
        wav.file.read((byte *)&buffer, buffer_bytes_size);
        file_bytes_read = 0;
        buffer_samples_read = 0;
    }

    int get_sample()
    {
        if (!is_playing)
        {
            return 0;
        }

        if (file_bytes_read == wav.header.data_size)
        {
            is_playing = false;

            return 0;
        }

        if (buffer_samples_read != buffer_bytes_size / wav.sample_bytes_size)
        {
            int sample = buffer[buffer_samples_read];
            buffer_samples_read += 1;

            return sample;
        }

        file_bytes_read += buffer_bytes_size;

        if (file_bytes_read + (SAMPLE_AMMOUNT * wav.sample_bytes_size) >= wav.header.data_size)
        {
            buffer_bytes_size = wav.header.data_size - file_bytes_read;
        }

        else
        {
            buffer_bytes_size = SAMPLE_AMMOUNT * wav.sample_bytes_size;
        }

        wav.file.read((byte *)&buffer, buffer_bytes_size);
        int sample = buffer[0];
        buffer_samples_read = 1;

        return sample;
    }

    void print_wav_data()
    {
        Serial.print("riff: ");
        Serial.print(wav.header.riff[0]);
        Serial.print(wav.header.riff[1]);
        Serial.print(wav.header.riff[2]);
        Serial.println(wav.header.riff[3]);

        Serial.print("fmt: ");
        Serial.print(wav.header.fmt[0]);
        Serial.print(wav.header.fmt[1]);
        Serial.print(wav.header.fmt[2]);
        Serial.println(wav.header.fmt[3]);

        Serial.print("size: ");
        Serial.println(wav.header.size);

        Serial.print("wave: ");
        Serial.print(wav.header.wave[0]);
        Serial.print(wav.header.wave[1]);
        Serial.print(wav.header.wave[2]);
        Serial.println(wav.header.wave[3]);

        Serial.print("fmt: ");
        Serial.print(wav.header.fmt[0]);
        Serial.print(wav.header.fmt[1]);
        Serial.print(wav.header.fmt[2]);
        Serial.println(wav.header.fmt[3]);

        Serial.print("rest_size: ");
        Serial.println(wav.header.rest_size);

        Serial.print("format: ");
        Serial.println(wav.header.format);

        Serial.print("channels: ");
        Serial.println(wav.header.channels);

        Serial.print("sample_rate: ");
        Serial.println(wav.header.sample_rate);

        Serial.print("byte_rate: ");
        Serial.println(wav.header.byte_rate);

        Serial.print("block_align: ");
        Serial.println(wav.header.block_align);

        Serial.print("bits_per_sample: ");
        Serial.println(wav.header.bits_per_sample);

        Serial.print("data: ");
        Serial.print(wav.header.data[0]);
        Serial.print(wav.header.data[1]);
        Serial.print(wav.header.data[2]);
        Serial.println(wav.header.data[3]);

        Serial.print("data_size: ");
        Serial.println(wav.header.data_size);
    }
};

player_t player;