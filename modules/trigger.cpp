#include "Arduino.h"
#include <../modules/button.cpp>

struct trigger_t
{
    int loop_count;
    int threshold;
    int resolution;
    int peak_sample;
    int start_scan_time;
    int elapsed_scan_time;
    int dynamic;

    void scan_sample(int sample)
    {
        if (sample < threshold * 10)
            return;

        if (loop_count == 0)
        {
            start_scan_time = millis();
            peak_sample = sample;
        }

        elapsed_scan_time = millis() - start_scan_time;

        if (elapsed_scan_time > resolution)
        {
            loop_count = 0;

            float dynamic_multiplier = peak_sample / 4096.0 * (pow((4096.0 / peak_sample), (1.0 - (dynamic / 100.0))));

            // could return the dynamic multiplier to clear dependecy
            player.trigger_sample(dynamic_multiplier);

            return;
        }

        if (sample > peak_sample)
        {
            peak_sample = sample;
        }

        loop_count++;
    }
};

trigger_t trigger = {
    .loop_count = 0,
};
