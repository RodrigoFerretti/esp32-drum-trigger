#include "Arduino.h"
#include <../modules/player.cpp>

struct button_t
{
    int pin;
    bool state;
    player_t *player;

    void setup(player_t *_player)
    {
        pinMode(pin, INPUT_PULLUP);
        player = _player;
    }

    void read()
    {
        bool read = digitalRead(pin);

        if (read < state)
        {
            player->trigger_sample();
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