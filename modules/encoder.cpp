#include "Arduino.h"
#include <../modules/player.cpp>

struct encoder_t
{
    int a_pin;
    int b_pin;
    int c_pin;
    bool a_read;
    bool b_read;
    bool c_read;
    bool c_read_before;
    float output_value;
    float max_value;
    float min_value;
    float step_value;
    bool rising;
    bool apply;

    void setup()
    {
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
        pinMode(c_pin, INPUT_PULLUP);
    }

    void read()
    {
        a_read = digitalRead(a_pin);
        b_read = digitalRead(b_pin);

        if ((a_read) & (b_read))

            return;

        apply = true;

        if ((!a_read) & (!b_read))
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

        if ((rising) & (output_value < max_value))
        {
            output_value += step_value;

            return;
        }

        if ((!rising) & (output_value > min_value))
        {
            output_value -= step_value;
        }
    }

    void read_buttom()
    {
        c_read = digitalRead(c_pin);

        if (c_read < c_read_before)
        {
            player_kick.trigger_sample();

            Serial.println("click");
        }

        if (c_read > c_read_before)
        {
            Serial.println("release");
        }

        c_read_before = c_read;
    }
};

encoder_t encoder_left = {
    .a_pin = 4,
    .b_pin = 16,
    .c_pin = 19,
    .a_read = true,
    .b_read = true,
    .c_read = true,
    .c_read_before = true,
    .output_value = 1.00,
    .max_value = 1.00,
    .min_value = 0.00,
    .step_value = 0.05,
    .rising = true,
    .apply = false,
};

encoder_t encoder_right = {
    .a_pin = 22,
    .b_pin = 23,
    .c_pin = 32,
    .a_read = true,
    .b_read = true,
    .c_read = true,
    .c_read_before = true,
    .output_value = 1.00,
    .max_value = 1.00,
    .min_value = 0.00,
    .step_value = 0.05,
    .rising = true,
    .apply = false,
};
