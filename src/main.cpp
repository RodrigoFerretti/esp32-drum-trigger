#include "Arduino.h"
#include "driver/i2s.h"
#include "hellodrum.h"

struct i2s_t
{
	i2s_port_t port;
	esp_err_t status;
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
	.status = ESP_OK,
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
	int pin;
};

piezo_t piezo = {
	.sample = 0,
	.pin = 26,
};

struct gate_scan_t
{
	int sample_threshold;
	int peak_sample;
	int loop_count;
	int start_time;
	int elapsed_time;
	int max_time;
};

struct gate_active_t
{
	int start_time;
	int elapsed_time;
	int max_time;
};

struct gate_t
{
	gate_scan_t scan;
	gate_active_t active;
	bool is_active;
};

gate_t gate_piezo = {
	.scan = {
		.sample_threshold = 0,
		.peak_sample = 0,
		.loop_count = 0,
		.start_time = 0,
		.elapsed_time = 0,
		.max_time = 0,
	},
	.active = {
		.start_time = 0,
		.elapsed_time = 0,
		.max_time = 700,
	},
	.is_active = false,
};

struct high_pass_t
{
	int EMA_s;
	float EMA_a;
};

high_pass_t high_pass_piezo = {
	.EMA_s = 0,
	.EMA_a = 0.2,
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

	i2s.status = i2s_driver_install(i2s.port, &i2s_config, 0, NULL);

	if (i2s.status != ESP_OK)
	{
		Serial.printf("Failed installing driver: %d\n", i2s.status);
		while (true)
			;
	}

	i2s.status = i2s_set_pin(i2s.port, &pin_config);

	if (i2s.status != ESP_OK)
	{
		Serial.printf("Failed setting pin: %d\n", i2s.status);
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

void apply_high_pass(high_pass_t *high_pass, int *sample)
{
	(*high_pass).EMA_s = ((*high_pass).EMA_a * *sample) + ((1 - (*high_pass).EMA_a) * (*high_pass).EMA_s);

	*sample = *sample - (*high_pass).EMA_s;
};

void apply_gate(gate_t gate, int *scan_sample, int *apply_sample)
{
	if (*scan_sample > gate.scan.sample_threshold)
	{
		if (gate.scan.loop_count == 0)
		{
			gate.scan.start_time = millis();
			gate.scan.peak_sample = *scan_sample;
		}

		gate.scan.elapsed_time = millis() - gate.scan.start_time;

		if (gate.scan.elapsed_time < gate.scan.max_time)
		{
			if (*scan_sample > gate.scan.peak_sample)
			{
				gate.scan.peak_sample = *scan_sample;
			}

			gate.scan.loop_count++;
		}

		else
		{
			gate.scan.loop_count = 0;
			gate.active.start_time = millis();
			gate.is_active = true;
		}
	}

	if (gate.is_active)
	{
		gate.active.elapsed_time = millis() - gate.active.start_time;

		if (gate.active.elapsed_time < gate.active.max_time)
		{
			*apply_sample = (float)1 - (gate.active.elapsed_time / gate.active.max_time);
		}

		else
		{
			*apply_sample = 0;
			gate.is_active = false;
		}
	}
}

void stream_i2s()
{
	i2s.status = i2s_read(i2s.port, &i2s.rx_buffer[0], i2s.size_to_read, &i2s.size_read, portMAX_DELAY);

	if (i2s.status == ESP_OK && i2s.size_read == i2s.size_to_read)
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
	apply_high_pass(&high_pass_piezo, &piezo.sample);
	apply_gate(gate_piezo, &piezo.sample, &i2s.output_ratio);
}
