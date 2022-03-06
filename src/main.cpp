#include "Arduino.h"
#include <../modules/i2s.cpp>
#include <../modules/piezo.cpp>
#include <../modules/trigger.cpp>
#include <../modules/high-pass.cpp>

void setup()
{
    Serial.begin(115200);
    sd.setup();
    sample_loader.setup();
    sample_loader.navigate_down();
    player.setup();
    button.setup(&player);
    equalizer.setup();
    i2s.setup();
}

void loop()
{
    piezo.sample = analogRead(piezo.pin);
    high_pass_piezo.filter_sample(&piezo.sample);
    high_pass_piezo.filter_sample(&piezo.sample);
    trigger_piezo.scan_sample(piezo.sample);
    i2s.input_multiplier = trigger_piezo.get_gate_multiplier();
    button.read();
    i2s.stream();
}
