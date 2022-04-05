#include "Arduino.h"

struct encoder_t
{
    int a_pin;
    int b_pin;
    float out_value;
    float max_value;
    float min_value;
    float stp_value;
    bool rising;
    bool apply;
    int states[4];
    int samples_read;

    void setup()
    {
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
    }

    void read()
    {
        bool a_state = digitalRead(a_pin);
        bool b_state = digitalRead(b_pin);

        if ((a_state) & (b_state))
        {
            samples_read = 0;

            return;
        }

        if ((a_state | b_state) & (samples_read == 0))
        {
            states[0] = a_state;
            states[1] = b_state;
            samples_read = 2;
        }

        if ((a_state | b_state) & (samples_read == 2))
        {
            states[2] = a_state;
            states[3] = b_state;
            samples_read = 4;
        }

        if ((states[0] == 1) & (states[3] == 1))
        {
            rising = true;
            apply = true;
        }

        if ((states[0] == 0) & (states[3] == 0))
        {
            rising = false;
            apply = true;
        }

        states[2] = states[0];
        states[3] = states[1];
        samples_read = 2;

        if (!apply)

            return;

        apply = false;

        if ((rising) & (out_value < max_value))
        {
            out_value += stp_value;

            return;
        }

        if ((!rising) & (out_value > min_value))
        {
            out_value -= stp_value;
        }
    }
};

encoder_t encoder_threshold = {
    .a_pin = 4,
    .b_pin = 2,
    .out_value = 1.00,
    .max_value = 1.00,
    .min_value = 0.00,
    .stp_value = 0.05,
};