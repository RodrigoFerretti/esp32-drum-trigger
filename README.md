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


## Must know

$$ max~volume = 20 * log_{~10~} (~bit~depth~max~value - 1~) $$
$$ times~can~divide~by~two = \frac{max~volume}{6db} $$

### Example

bit_depth = 24

so...

bit_depth_max_value = 16,777,216

calculating...

max_volume = 20 * log(10) 16,777,215 => ~144dB

times_can_divide_by_two = 144 / 6 -> 24


-000.00000db		sample * 1
-000.09375db		sample * ((1 - 0.984375) / 2) + 0.984375 -> 0.9921875
-000.18750db		sample * ((1 - 0.96875) / 2) + 0.96875 -> 0.984375
-000.37500db		sample * ((1 - 0.9375) / 2) + 0.9375 -> 0.96875
-000.75000db		sample * ((1 - 0.875) / 2) + 0.875 -> 0.9375
-001.50000db		sample * ((1 - 0.75) / 2) + 0.75 -> 0.875
-003.00000db		sample * ((1 - 0.5) / 2) + 0.5 -> 0.75
-006.00000db		sample * 0.5
-012.00000db		sample * 0.25
-018.00000db		sample * 0.125
-024.00000db		sample * 0.0625 
-030.00000db		sample * 0.03125

step_volume = 0.09375
step_diviser = 0.015625

