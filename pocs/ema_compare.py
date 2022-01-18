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
    "EMA_a": 0.05,
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
