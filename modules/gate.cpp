#include "Arduino.h"

struct gate_t
{
    float release;
    float range;
    int start_active_time;
    int elapsed_active_time;
    bool is_active;

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