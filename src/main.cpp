#include "Arduino.h"
#include <../modules/i2s.cpp>
#include <../modules/piezo.cpp>
#include <../modules/trigger.cpp>
#include <../modules/high-pass.cpp>

void setup()
{
    Serial.begin(115200);

    preferences.begin("trigger", false);
    trigger.threshold = preferences.getInt("TH", 100);
    trigger.resolution = preferences.getInt("RS", 20);
    trigger.dynamic = preferences.getInt("DY", 100);
    player.position = preferences.getInt("position", 0);

    sd.setup();

    button_down.setup();
    button_up.setup();
    button_play.setup();

    player.setup();

    encoder_threshold.setup(trigger.threshold);
    encoder_resolution.setup(trigger.resolution);
    encoder_dynamic.setup(trigger.dynamic);

    i2s.setup();

    lcd_setup(trigger.threshold, trigger.resolution, trigger.dynamic, player.directory.name(), player.position);
}

void loop()
{
    button_down.read();
    button_up.read();
    button_play.read();
    
    encoder_threshold.read();
    encoder_resolution.read();
    encoder_dynamic.read();

    trigger.threshold = encoder_threshold.out_value;
    trigger.resolution = encoder_resolution.out_value;
    trigger.dynamic = encoder_dynamic.out_value;

    piezo.sample = analogRead(piezo.pin);
    high_pass_piezo.filter_sample(&piezo.sample);
    high_pass_piezo.filter_sample(&piezo.sample);
    trigger.scan_sample(piezo.sample);
    i2s.stream();
}
