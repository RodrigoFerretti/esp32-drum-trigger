# esp32Audio

Project design for making a gate for kick and snare drum mics feeded by piezo triggers in ESP32 and I2S PCM boards.

## PCM1802 ADC Pinout
```
| ADC   | ESP32 |
|-------|-------|
| 3.3V  | 3.3V  |
| GND   | GND   |
| SCK   | IO0   |
| LRC   | IO16  |
| OUT   | IO18  |
| BCK   | IO5   |
| -     | -     |
| LIN   | LIN   |
| AGND  | AGND  |
| RIN   | RIN   |
| -     | -     |
| FMY   | -     |
| MD1   | -     |
| MD0   | -     |
| GND   | -     |
| 3.3   | -     |
| +5V   | -     |

```

## UDA1334A DAC Pinout
```
| DAC   | ESP32 |
|-------|-------|
| VIN   | -     |
| 3V0   | 3V3   |
| GND   | GND   |
| WSEL  | IO16  |
| DIN   | IO17  |
| BCKL  | IO5   |
| Lout  | Lout  |
| AGND  | AGND  |
| Rout  | Rout  |
| -     | -     |
| SCLK  | IO0   |    
| SF1   | -     |
| MUTE  | -     |
| SF0   | -     |
| PLL   | -     |
| DEEM  | -     |

```

## LCD 4x20
```
| LCD   | ESP32 |
|-------|-------|
| GND   | GND   |
| VCC   | 5V    |
| SDA   | IO21  |
| SCL   | IO22  |
```

## SD Card Reader
```
| SD    | ESP32 |
|-------|-------|
| CS    | IO12  |
| SCK   | IO14  |
| MOSI  | IO26  |
| MISO  | IO27  |
| VCC   | 5V    |
| GND   | GND   |

```

## Encoders
```
| ENCOD | ESP32 |
|-------|-------|
| A     | IO19  |
| B     | IO23  |
| -     | -     |
| A     | IO15  |
| B     | IO2   |
| -     | -     |
| A     | IO13  |
| B     | IO25  |

```

## Buttons
```
| BUTTO | ESP32 |
|-------|-------|
| GND   | GND   |
| HOT   | IO33  |
| -     | -     |
| GND   | GND   |
| HOT   | IO32  |
| -     | -     |
| GND   | GND   |
| HOT   | IO3   |

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

### Audio Sample Buffer

```python
from scipy.io.wavfile import read
import numpy

a = read("kick.wav")
b = numpy.array(a[1],dtype=int)
c = [sample[0] for sample in b][:8704]

print(c)
```

```cpp
int input_buffer[64];

int output_buffer[64];

int input_buffer[64];

int output_buffer[64];

float a_coeffs[3];

float b_coeffs[3];

```
