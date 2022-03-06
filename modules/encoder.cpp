#include "Arduino.h"

struct encoder_t
{
    int a_pin;
    int b_pin;
    float out_value;
    float max_value;
    float min_value;
    float stp_value;
    bool a_state;
    bool b_state;
    bool rising;
    bool apply;

    void setup()
    {
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
    }

    void read()
    {
        a_state = digitalRead(a_pin);
        b_state = digitalRead(b_pin);

        if ((a_state) & (b_state))

            return;

        apply = true;

        if ((!a_state) & (!b_state))
        {
            rising = true;

            return;
        }

        rising = false;
    }

    void set_output_value()
    {
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

encoder_t encoder_left = {
    .a_pin = 4,
    .b_pin = 16,
    .out_value = 1.00,
    .max_value = 1.00,
    .min_value = 0.00,
    .stp_value = 0.05,
};
