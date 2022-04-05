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

    void setup(File files[4])
    {
        for (int i = 0; i < 4; i++)
        {
            wav_file_t wav_file;
            wavs[i] = wav_file;
            File file = files[i];
            wavs[i].setup(file);
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
};

player_t player;
