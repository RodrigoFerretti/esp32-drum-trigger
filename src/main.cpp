#include "Arduino.h"
#include <../modules/i2s.cpp>
#include <../modules/piezo.cpp>
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
    if (button_down.read())
        player.navigate_down();
    if (button_up.read())
        player.navigate_up();
    if (button_play.read())
        player.trigger_sample(1);

    trigger.threshold = encoder_threshold.read() * 10;
    trigger.resolution = encoder_resolution.read();
    trigger.dynamic = encoder_dynamic.read();

    // trigger.scan_sample(piezo.sample);
    // i2s.stream();
}
