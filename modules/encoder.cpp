#include "Arduino.h"
#include <../modules/lcd.cpp>
#include <../modules/preferences.cpp>

struct encoder_t
{
    const char *lcd_name;
    int lcd_position;
    int a_pin;
    int b_pin;
    float max_value;
    float min_value;
    float stp_value;
    float out_value;
    bool rising;
    bool apply;
    int states[4];
    int samples_read;

    void setup(float _out_value)
    {
        out_value = _out_value;
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
    }

    void read()
    {
        bool a_state = digitalRead(a_pin);
        bool b_state = digitalRead(b_pin);

        if ((a_state) & (b_state))
        {
            set_output_value();
            samples_read = 0;

            return;
        }

        if ((a_state | b_state) & (samples_read == 0))
        {
            states[0] = a_state;
            states[1] = b_state;
            samples_read = 2;
        }

        if ((a_state | b_state) & (a_state != states[0]) & (b_state != states[1]) & (samples_read == 2))
        {
            states[2] = a_state;
            states[3] = b_state;
            samples_read = 4;
        }
    }

    void set_output_value()
    {
        if ((states[0] == 1) & (states[1] == 0) & (states[2] == 0) & (states[3] == 1) & (samples_read == 4))
        {
            rising = true;
            apply = true;
        }

        if ((states[0] == 0) & (states[1] == 1) & (states[2] == 1) & (states[3] == 0) & (samples_read == 4))
        {
            rising = false;
            apply = true;
        }

        if (!apply)

            return;

        apply = false;

        if ((rising) & (out_value < max_value))
        {
            out_value += stp_value;
            display_parameter(lcd_name, out_value, lcd_position);
            preferences.putInt(lcd_name, out_value);

            return;
        }

        if ((!rising) & (out_value > min_value))
        {
            out_value -= stp_value;
            display_parameter(lcd_name, out_value, lcd_position);
            preferences.putInt(lcd_name, out_value);
        }
    }
};

encoder_t encoder_threshold = {
    .lcd_name = "TH",
    .lcd_position = 0,
    .a_pin = 19,
    .b_pin = 23,
    .max_value = 400,
    .min_value = 0,
    .stp_value = 1,
};

encoder_t encoder_resolution = {
    .lcd_name = "RS",
    .lcd_position = 7,
    .a_pin = 15,
    .b_pin = 2,
    .max_value = 100,
    .min_value = 0,
    .stp_value = 1,
};

encoder_t encoder_dynamic = {
    .lcd_name = "DY",
    .lcd_position = 14,
    .a_pin = 13,
    .b_pin = 25,
    .max_value = 100,
    .min_value = 0,
    .stp_value = 1,
};