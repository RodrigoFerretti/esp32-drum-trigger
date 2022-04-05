#include "Arduino.h"
#include <../modules/i2s.cpp>
#include <../modules/piezo.cpp>
#include <../modules/trigger.cpp>
#include <../modules/high-pass.cpp>
#include <../modules/encoder.cpp>
// #include <../modules/preferences.cpp>

void setup()
{
    Serial.begin(115200);
    // preferences.begin("trigger", false);
    // trigger.threshold = preferences.getInt("threshold", 100);
    // trigger.max_scan_time = preferences.getInt("resolution", 20);
    // trigger.dynamic_tracking = preferences.getInt("dynamic", 100);
    // sample_loader.position = preferences.getInt("position", 0);
    // lcd.init();
    // lcd.backlight();
    sd.setup();
    button.setup();
    sample_loader.setup();
    encoder_threshold.setup();
    i2s.setup();
}

void loop()
{
    encoder_threshold.read();
    trigger.threshold = encoder_threshold.out_value;
    piezo.sample = analogRead(piezo.pin);
    high_pass_piezo.filter_sample(&piezo.sample);
    high_pass_piezo.filter_sample(&piezo.sample);
    trigger.scan_sample(piezo.sample);
    i2s.stream();
}
