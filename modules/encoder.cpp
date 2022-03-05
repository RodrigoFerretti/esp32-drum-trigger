#include "Arduino.h"
#include <../modules/player.cpp>

struct encoder_t
{
    int a_pin;
    int b_pin;
    int c_pin;
    float out_value;
    float max_value;
    float min_value;
    float stp_value;
    bool a_state;
    bool b_state;
    bool c_state;
    bool rising;
    bool apply;
    player_t *player;

    void setup(player_t *_player)
    {
        pinMode(a_pin, INPUT_PULLUP);
        pinMode(b_pin, INPUT_PULLUP);
        pinMode(c_pin, INPUT_PULLUP);

        player = _player;
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

    void read_buttom()
    {
        bool c_read = digitalRead(c_pin);

        if (c_read < c_state)
        {
            player->trigger_sample();
        }

        if (c_read > c_state)
        {
            Serial.println("release");
        }

        c_state = c_read;
    }
};

encoder_t encoder_left = {
    .a_pin = 4,
    .b_pin = 16,
    .c_pin = 23,
    .out_value = 1.00,
    .max_value = 1.00,
    .min_value = 0.00,
    .stp_value = 0.05,
};
