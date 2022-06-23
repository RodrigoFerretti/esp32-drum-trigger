#include "Arduino.h"
#include <../modules/player.cpp>

struct button_t
{
    int pin;
    bool state;

    void setup()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    bool read()
    {
        bool read = digitalRead(pin);

        if (read < state)
        {
            state = read;

            return 1;
        }

        state = read;

        return 0;
    }
};

button_t button_play = {
    .pin = 32,
};

button_t button_down = {
    .pin = 33,
};

button_t button_up = {
    .pin = 3,
};