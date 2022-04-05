#include "Arduino.h"
// #include <../modules/sample-loader.cpp>

struct trigger_t
{
    int loop_count;
    int threshold;
    int max_scan_time;
    int peak_sample;
    int start_scan_time;
    int elapsed_scan_time;
    int dynamic_tracking;

    void scan_sample(int sample)
    {
        if (sample < threshold)
            return;

        if (loop_count == 0)
        {
            start_scan_time = millis();
            peak_sample = sample;
        }

        elapsed_scan_time = millis() - start_scan_time;

        if (elapsed_scan_time > max_scan_time)
        {
            loop_count = 0;

            // player.trigger_sample();

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
