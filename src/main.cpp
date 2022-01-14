#include "Arduino.h"
#include "driver/i2s.h"

const i2s_port_t I2S_PORT = I2S_NUM_0;

int rxbuf[128], txbuf[128];

size_t readsize = 0;

struct Piezo
{
	int sample;
	int sampleBefore = 0;
	int hitTime;
	int hitTimeBefore = 0;
	const int threshold = 0;
	const int timeResolution = 40;
	const int pin = 26;
};

Piezo piezo;

struct Gate
{
	int hitTime;
	int time;
	int timeDifference;
	float outputRatio = 1;
	float ratioFloor = 0.2;
	int timeBase = 700;
	int timeFloor = 900;
	byte isActive = LOW;
};

Gate gate;

struct HighPass
{
	int outputSample;
	float EMA_s = 0;
	float EMA_a = 0.04;
};

HighPass highPass;

HighPass filter(int inputSample, HighPass _highPass)
{
	_highPass.EMA_s = (_highPass.EMA_a * inputSample) + ((1 - _highPass.EMA_a) * _highPass.EMA_s);
	_highPass.outputSample = inputSample - _highPass.EMA_s;

	if (_highPass.outputSample < 0)
	{
		_highPass.outputSample = 0;
	}

	return _highPass;
};

void gating()
{
	highPass = filter(analogRead(piezo.pin), highPass);
	piezo.sample = highPass.outputSample;

	highPass = filter(analogRead(piezo.pin), highPass);
	piezo.sample = highPass.outputSample;

	if ((piezo.sample > 0) && (piezo.sampleBefore == 0) && (piezo.sample > piezo.threshold))
	{
		piezo.hitTime = millis();

		if (piezo.hitTime - piezo.hitTimeBefore > piezo.timeResolution)
		{
			gate.hitTime = millis();
			gate.isActive = HIGH;
			piezo.hitTimeBefore = piezo.hitTime;
			Serial.println(piezo.sample);
		}

		piezo.hitTimeBefore = piezo.hitTime;
	}

	piezo.sampleBefore = piezo.sample;

	if (gate.isActive == HIGH)
	{
		gate.time = millis();
		gate.timeDifference = gate.time - gate.hitTime;
		if (gate.timeDifference < gate.timeBase)
		{
			gate.outputRatio = (float)1 - ((float)gate.timeDifference / gate.timeFloor);
		}
		else
		{
			gate.outputRatio = gate.ratioFloor;
			gate.isActive = LOW;
		}
	}
}

void setup()
{
	Serial.begin(115200);
	esp_err_t err;

	const i2s_config_t i2s_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
		.sample_rate = 44100,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 8,
		.dma_buf_len = 256,
	};

	const i2s_pin_config_t pin_config = {
		.bck_io_num = 19,	// Serial Clock (SCK)
		.ws_io_num = 5,		// Word Select (WS)
		.data_out_num = 18, // not used (only for speakers)
		.data_in_num = 21	// Serial Data (SD)
	};

	err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
	if (err != ESP_OK)
	{
		Serial.printf("Failed installing driver: %d\n", err);
		while (true)
			;
	}

	err = i2s_set_pin(I2S_PORT, &pin_config);
	if (err != ESP_OK)
	{
		Serial.printf("Failed setting pin: %d\n", err);
		while (true)
			;
	}

	Serial.println("I2S driver installed.");

	REG_WRITE(PIN_CTRL, 0b111111110000);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}

void loop()
{
	gating();
	
	esp_err_t rxfb = i2s_read(I2S_PORT, &rxbuf[0], 128 * 4, &readsize, portMAX_DELAY);
	if (rxfb == ESP_OK && readsize == 128 * 4)
	{

		for (int i = 0; i < 128; i = i + 2)
		{
			txbuf[i] = (int)rxbuf[i] * gate.outputRatio;
			txbuf[i + 1] = (int)rxbuf[i + 1] * gate.outputRatio;
		}

		i2s_write(I2S_PORT, &txbuf[0], 128 * 4, &readsize, portMAX_DELAY);
	}
}
