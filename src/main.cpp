#include "Arduino.h"
#include "driver/i2s.h"

struct i2s_t
{
	i2s_port_t port;
	esp_err_t error;
	i2s_bits_per_sample_t bit_depth;
	int sample_rate;
	int dma_buff_count;
	int dma_buff_len;
	int dout_pin;
	int din_pin;
	int bck_pin;
	int lck_pin;
	int rx_buffer[128];
	int tx_buffer[128];
	int size_to_read;
	size_t size_read;
	int output_ratio;
};

i2s_t i2s = {
	.port = I2S_NUM_0,
	.error = ESP_OK,
	.bit_depth = (i2s_bits_per_sample_t)24,
	.sample_rate = 44100,
	.dma_buff_count = 8,
	.dma_buff_len = 256,
	.dout_pin = 18,
	.din_pin = 21,
	.bck_pin = 19,
	.lck_pin = 5,
	.rx_buffer = {},
	.tx_buffer = {},
	.size_to_read = 128 * (sizeof(uint8_t) * 4),
	.size_read = 0,
	.output_ratio = 1,
};

struct piezo_t
{
	int sample;
	int sample_before;
	int pin;
};

piezo_t piezo = {
	.sample = 0,
	.sample_before = 0,
	.pin = 26,
};

struct gate_t
{
	int sample_hit_time;
	int sample_hit_time_before;
	int sample_time_resolution;
	int start_time;
	int current_time;
	int time_difference;
	int threshold;
	float ratio_floor;
	int time_base;
	int time_floor;
	bool is_active;
};

gate_t gate_piezo = {
	.sample_hit_time = 0,
	.sample_hit_time_before = 0,
	.sample_time_resolution = 40,
	.start_time = 0,
	.current_time = 0,
	.time_difference = 0,
	.threshold = 0,
	.ratio_floor = 0.2,
	.time_base = 700,
	.time_floor = 900,
	.is_active = false,
};

struct high_pass_t
{
	int output_sample;
	float EMA_s;
	float EMA_a;
};

high_pass_t high_pass_piezo = {
	.output_sample = 0,
	.EMA_s = 0,
	.EMA_a = 0.04,
};

void configure_i2s()
{
	const i2s_config_t i2s_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
		.sample_rate = i2s.sample_rate,
		.bits_per_sample = i2s.bit_depth,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = i2s.dma_buff_count,
		.dma_buf_len = i2s.dma_buff_len,
	};

	const i2s_pin_config_t pin_config = {
		.bck_io_num = i2s.bck_pin,
		.ws_io_num = i2s.lck_pin,
		.data_out_num = i2s.dout_pin,
		.data_in_num = i2s.din_pin,
	};

	i2s.error = i2s_driver_install(i2s.port, &i2s_config, 0, NULL);

	if (i2s.error != ESP_OK)
	{
		Serial.printf("Failed installing driver: %d\n", i2s.error);
		while (true)
			;
	}

	i2s.error = i2s_set_pin(i2s.port, &pin_config);

	if (i2s.error != ESP_OK)
	{
		Serial.printf("Failed setting pin: %d\n", i2s.error);
		while (true)
			;
	}

	Serial.println("I2S driver installed.");

	REG_WRITE(PIN_CTRL, 0b111111110000);

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}

void setup()
{
	Serial.begin(115200);

	configure_i2s();
}

void apply_high_pass(high_pass_t high_pass, int *sample)
{
	high_pass.EMA_s = (high_pass.EMA_a * *sample) + ((1 - high_pass.EMA_a) * high_pass.EMA_s);

	*sample = *sample - high_pass.EMA_s;

	if (*sample < 0)
	{
		*sample = 0;
	}
};

void apply_gate(gate_t gate, int *read_sample, int *read_sample_before, int *sample_to_apply)
{
	if ((*read_sample > gate.threshold) && (*read_sample_before == gate.threshold))
	{
		gate.sample_hit_time = millis();

		if (gate.sample_hit_time - gate.sample_hit_time_before > gate.sample_time_resolution)
		{
			gate.start_time = millis();
			gate.is_active = true;
			Serial.println(*read_sample);
		}

		gate.sample_hit_time_before = gate.sample_hit_time;
	}

	if (gate.is_active)
	{
		gate.current_time = millis();

		gate.time_difference = gate.current_time - gate.start_time;

		if (gate.time_difference < gate.time_base)
		{
			*sample_to_apply = (float)1 - (gate.time_difference / gate.time_floor);
		}
		else
		{
			*sample_to_apply = gate.ratio_floor;
			gate.is_active = false;
		}
	}
}

void stream_i2s()
{
	i2s.error = i2s_read(i2s.port, &i2s.rx_buffer[0], i2s.size_to_read, &i2s.size_read, portMAX_DELAY);

	if (i2s.error == ESP_OK && i2s.size_read == i2s.size_to_read)
	{

		for (int i = 0; i < 128; i = i + 2)
		{
			i2s.tx_buffer[i] = (int)i2s.rx_buffer[i] * i2s.output_ratio;
			i2s.tx_buffer[i + 1] = (int)i2s.rx_buffer[i + 1] * i2s.output_ratio;
		}

		i2s_write(i2s.port, &i2s.tx_buffer[0], i2s.size_to_read, &i2s.size_read, portMAX_DELAY);
	}
}

void loop()
{
	piezo.sample = analogRead(piezo.pin);

	apply_high_pass(high_pass_piezo, &piezo.sample);

	apply_gate(gate_piezo, &piezo.sample, &piezo.sample_before, &i2s.output_ratio);

	stream_i2s();

	piezo.sample_before = piezo.sample;
}
