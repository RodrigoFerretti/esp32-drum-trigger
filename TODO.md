### ESP32:

-   switch io34 or io35 pin to input piezo, io4 for last button
-   configure methods for buttons
-   develop dynamic algorithm logic
-   send dynamic to player

### WITHOUT DRUMS:

-   find wav's max_value for 16bits then define normalization multiplier

### WITH DRUMS:

-   find piezo's max_value input with current configuration
-   find threshold friendly_range between 0 and piezo's max_value
-   find max_scan_time friendly range between 0 and 200 ms
-   find dynamic_tracking multiplier_rule between 0 and 1 for piezo's max_value and 0 for threshold

### Hardware

-   measure approximate box size
-   XLR input for trigger with resistor
-   TRS output with isolator
-   5V power input with isolator
