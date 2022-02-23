#include "Arduino.h"
#include <../modules/i2s.cpp>
#include <../modules/piezo.cpp>
#include <../modules/trigger.cpp>
#include <../modules/high-pass.cpp>

void setup()
{
    Serial.begin(115200);
    i2s.setup();
    player_kick.setup();
    encoder_left.setup();
    encoder_right.setup();
}

void loop()
{
    piezo_left.sample = analogRead(piezo_left.pin);
    high_pass_piezo_left.filter_sample(&piezo_left.sample);
    high_pass_piezo_left.filter_sample(&piezo_left.sample);
    trigger_piezo_left.scan_sample(piezo_left.sample);
    i2s.left_gate_multiplier = trigger_piezo_left.get_gate_multiplier();

    piezo_right.sample = analogRead(piezo_right.pin);
    high_pass_piezo_right.filter_sample(&piezo_right.sample);
    high_pass_piezo_right.filter_sample(&piezo_right.sample);
    trigger_piezo_right.scan_sample(piezo_right.sample);
    i2s.right_gate_multiplier = trigger_piezo_right.get_gate_multiplier();

    encoder_left.read_buttom();
    encoder_right.read_buttom();

    i2s.stream();
}
