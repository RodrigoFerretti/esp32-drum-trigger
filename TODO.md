### ESP32:

-   setup 2 more encoders
-   encoder for trigger->threshold
-   encoder for trigger->max_scan_time
-   encoder for trigger->dynamic_tracking
-   trigger call player->trigger_sample and send dynamic_tracking multiplier
-   setup display for tracking values
-   store 3 values internally esp32

### WITHOUT DRUMS:

-   find wav's max_value for 16bits then define normalization multiplier
-   find piezo's max_value input with current configuration

### WITH DRUMS:

-   find threshold friendly_range between 0 and piezo's max_value
-   find max_scan_time friendly range between 0 and 200 ms
-   find dynamic_tracking multiplier_rule between 0 and 1 for piezo's max_value and 0 for threshold

### Hardware

-   measure approximate box size
-   XLR input for trigger with resistor
-   TRS output with isolator
-   5V power input with isolator