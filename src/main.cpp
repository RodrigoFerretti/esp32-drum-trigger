#include "Arduino.h"
#include "driver/i2s.h"

struct i2s_t
{
	i2s_port_t 	port;
	int 		rxBuf[128];
	int 		txBuf[128];
	int 		size;
	size_t 		readSize;
};

i2s_t i2s = {
	.port = I2S_NUM_0,
	.rxBuf = {},
	.txBuf = {},
	.size = 128 * sizeof(uint16_t),
	.readSize = 0,
};

struct Piezo
{
	int sample;
	int sampleBefore;
	int hitTime;
	int hitTimeBefore;
	int threshold;
	int timeResolution;
	int pin;
};

Piezo piezo = {
	.sample = 0,
	.sampleBefore = 0,
	.hitTime = 0,
	.hitTimeBefore = 0,
	.threshold = 0,
	.timeResolution = 40,
	.pin = 26,
};

struct Gate
{
	int 	hitTime;
	int 	time;
	int 	timeDifference;
	float 	outputRatio;
	float 	ratioFloor;
	int 	timeBase;
	int 	timeFloor;
	byte 	isActive;
};

Gate gate = {
	.hitTime = 0,
	.time = 0,
	.timeDifference = 0,
	.outputRatio = 1,
	.ratioFloor = 0.2,
	.timeBase = 700,
	.timeFloor = 900,
	.isActive = LOW,
};

struct HighPass
{
	int 	outputSample;
	float 	EMA_s;
	float 	EMA_a;
};

HighPass highPass = {
	.outputSample = 0,
	.EMA_s = 0,
	.EMA_a = 0.04,
};

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
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
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

	err = i2s_driver_install(i2s.port, &i2s_config, 0, NULL);
	if (err != ESP_OK)
	{
		Serial.printf("Failed installing driver: %d\n", err);
		while (true)
			;
	}

	err = i2s_set_pin(i2s.port, &pin_config);
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

	esp_err_t rxfb = i2s_read(i2s.port, &i2s.rxBuf[0], i2s.size, &i2s.readSize, portMAX_DELAY);
	if (rxfb == ESP_OK && i2s.readSize == i2s.size)
	{

		for (int i = 0; i < 128; i = i + 2)
		{
			i2s.txBuf[i] = (int)i2s.rxBuf[i] * gate.outputRatio;
			i2s.txBuf[i + 1] = (int)i2s.rxBuf[i + 1] * gate.outputRatio;
		}

		i2s_write(i2s.port, &i2s.txBuf[0], i2s.size, &i2s.readSize, portMAX_DELAY);
	}
}
