#include "Arduino.h"
#include <cstring>
#include <../modules/kick.cpp>

const int SAMPLE_SIZE = 8704;
const int SAMPLE_AMMOUT = 2;

struct player_t
{
    bool is_playing;
    int sample_size;
    int samples_read;
    int sample_number;
    int samples[SAMPLE_AMMOUT][SAMPLE_SIZE];

    void setup()
    {
        memcpy(samples[0], kick, sizeof(kick));
        memcpy(samples[1], kick, sizeof(kick));
    }

    void trigger_sample()
    {
        is_playing = true;
        samples_read = 0;
        sample_number = random(0, SAMPLE_AMMOUT);
    }

    int get_sample()
    {
        if (!is_playing)
        {
            return 0;
        }

        if (samples_read == sample_size)
        {
            is_playing = false;
            return 0;
        }

        int sample = samples[sample_number][samples_read];
        samples_read += 1;

        return sample;
    }
};

player_t player_kick = {
    .is_playing = false,
    .sample_size = SAMPLE_SIZE,
    .samples_read = 0,
    .sample_number = SAMPLE_AMMOUT,
    .samples = {},
};
