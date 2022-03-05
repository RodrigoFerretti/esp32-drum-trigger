#include "Arduino.h"

struct trigger_t
{
    int threshold;
    float release;
    float range;
    int max_scan_time;
    int loop_count;
    int peak_sample;
    int start_scan_time;
    int elapsed_scan_time;
    int start_active_time;
    int elapsed_active_time;
    bool is_active;

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
            start_active_time = millis();
            is_active = true;

            return;
        }

        if (sample > peak_sample)
        {
            peak_sample = sample;
        }

        loop_count++;
    }

    float get_gate_multiplier()
    {
        float gate_multiplier = pow(0.5, range / 6.0);

        if (!is_active)
            return gate_multiplier;

        elapsed_active_time = millis() - start_active_time;

        if (elapsed_active_time < release)
        {
            float to_remove = (float)(elapsed_active_time / (release / (1 - gate_multiplier)));

            return (float)1 - to_remove;
        }

        is_active = false;

        return gate_multiplier;
    }
};

trigger_t trigger_piezo = {
    .threshold = 100,
    .release = 444.0,
    .range = 12,
    .max_scan_time = 20,
    .loop_count = 0,
};
