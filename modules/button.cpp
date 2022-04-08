#include "Arduino.h"
#include <../modules/player.cpp>

struct button_t
{
    int pin;
    int type;
    bool state;

    void setup()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    void read()
    {
        bool read = digitalRead(pin);

        if ((read < state) & (type == 0))
        {
            player.trigger_sample(1);
        }

        if ((read < state) & (type == 1))
        {
            player.navigate_down();
        }

        if ((read < state) & (type == 2))
        {
            player.navigate_up();
        }

        state = read;
    }
};

button_t button_play = {
    .pin = 32,
    .type = 0,
};

button_t button_down = {
    .pin = 3,
    .type = 1,
};

button_t button_up = {
    .pin = 33,
    .type = 2,
};