### WITHOUT DRUMS:

-   find wav's max_value for 16bits then define normalization multiplier (something like 10000)

### Hardware

-   design box
-   mount box

### WITH DRUMS:

-   find piezo's max_value input with current configuration
-   find threshold friendly_range between 0 and piezo's max_value
-   find max_scan_time friendly range between 0 and 100 ms

### ESP32:

-   several minor refactors with tests
