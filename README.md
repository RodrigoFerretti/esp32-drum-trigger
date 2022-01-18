# esp32Audio

Project design for making a gate for kick and snare drum mics feeded by piezo triggers in ESP32 and I2S PCM boards.

## PCM1802 ADC Pinout

| ADC   | I2S   | ESP32 |
|-------|-------|-------|
| +5V   | VIN   | VIN   |
| 3.3V  | 3.3V  | 3V3   |
| GND   | GND   | GND   |
| DOUT  | DIN   | 21    |
| BCK   | BCK   | 19    |
| FSY   | 3.3V  | 3V3   |
| LRCK  | LCK   | 5     |
| POW   | 3.3V  | 3V3   |
| SCK   | MCLK  | 0     |

## PCM5102A DAC Pinout

| DAC   | I2S   | ESP32 |
|-------|-------|-------|
| VCC   | VIN   | VIN   |
| 3.3V  | 3.3V  | 3V3   |
| GND   | GND   | GND   |
| FLT   | GND   | GND   |
| DMP   | GND   | GND   |
| SCL   | MCLK  | 0     |
| BCK   | BCK   | 19    |
| DIN   | DOUT  | 18    |
| LCK   | LCK   | 5     |
| FMT   | GND   | GND   |
| XMT   | 3.3V  | 3V3   |

## Exponencial Moving Average Alpha - High Pass Filter

| EMA_a | High Pass Filter     |
|-------|----------------------|
| 0.05  | 0338.71 hz 6dB / oct |
| 0.1   | 0878.52 hz 6dB / oct |
| 0.2   | 2022.70 hz 6dB / oct |
| 0.3   | 3514.10 hz 6dB / oct |
