# esp32Audio

Project design for making a gate for kick and snare drum mics feeded by piezo triggers in ESP32 and I2S PCM boards.

## PCM1802 ADC Pinout
```
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
```

## PCM5102A DAC Pinout
```
| DAC   | I2S   | ESP32 |
|-------|-------|-------|
| VCC   | VIN   | VIN   |
| 3.3V  | 3.3V  | 3V3   |
| GND   | GND   | GND   |
| FLT   | GND   | 3V3   |
| DMP   | GND   |       |
| SCL   | MCLK  | 0     |
| BCK   | BCK   | 19    |
| DIN   | DOUT  | 18    |
| LCK   | LCK   | 5     |
| FMT   | GND   | GND   |
| XMT   | 3.3V  | 3V3   |
```

## Exponencial Moving Average Alpha - High Pass Filter
```
| EMA_a | High Pass Filter     |
|-------|----------------------|
| 0.05  | 0338.71 hz 6dB / oct |
| 0.1   | 0878.52 hz 6dB / oct |
| 0.2   | 2022.70 hz 6dB / oct |
| 0.3   | 3514.10 hz 6dB / oct |
```

Code used to generate raw and filtered samples to compare

```python
import numpy as np
import scipy.io.wavfile as wavf


def apply_high_pass(high_pass: object, read_sample: int) -> int:
	high_pass["EMA_s"] = int(
        high_pass["EMA_a"] * read_sample) + ((1 - high_pass["EMA_a"]) * high_pass["EMA_s"]
    )

	return read_sample - high_pass["EMA_s"]


sample_rate = 44100

original_array = np.random.uniform(-32768, 32767, 500000)

filtered_array = np.copy(original_array)

high_pass_test = {
    "EMA_a": 0.2,
    "EMA_s": 0
}

for i, sample in enumerate(original_array):
    filtered_array[i] = apply_high_pass(high_pass_test, sample)

wavf.write(
    f'filtered-{str(high_pass_test["EMA_a"])}.wav', 
    sample_rate, 
    filtered_array
)

wavf.write(
    f'original-{str(high_pass_test["EMA_a"])}.wav', 
    sample_rate, 
    original_array
)

```

## Must know

$$ max~volume = 20~log_{~10~} (~bit~depth~max~value - 1~) $$

$$ multiply~sample~by = 0.5^( \frac{db~to~remove}{6} ~^) $$

### Example

```
bit_depth = 24

so...

bit_depth_max_value = 16,777,216

calculating...

max_volume = 20 log(10) 16,777,215 => ~144dB

desired_db_reduction = 144db

multiply_sample_by = 0.5 ** (144 / 6)
```