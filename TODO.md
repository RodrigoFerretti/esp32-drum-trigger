### ESP32:

-   setup 2 more encoders, 3 total (trigger->threshold, trigger->max_scan_time, trigger->dynamic_tracking)
-   trigger call player->trigger_sample send dynamic_tracking multiplier and algorithm
-   store values from preferences lib to reboot
-   buttons for navigate up and down folder
-   buttom for hearing sample
-   ps: 3 encoders & 3 buttons
-   lcd output without bugs

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
