#include "Arduino.h"
#include <../modules/sample-loader.cpp>

struct button_t
{
    int pin;
    bool state;
    void setup()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    void read()
    {
        bool read = digitalRead(pin);

        if (read < state)
        {
            player.trigger_sample();
        }

        if (read > state)
        {
            Serial.println("release");
        }

        state = read;
    }
};


button_t button = {
    .pin = 23,
};