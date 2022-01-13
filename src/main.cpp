#include "Arduino.h"
#include "driver/i2s.h"
const i2s_port_t I2S_PORT = I2S_NUM_0;

int rxbuf[128], txbuf[128];

float l_in[64], r_in[64];
float l_out[64], r_out[64];

size_t readsize = 0;

int EMA_S = 0;
int highpass;

int piezoPin = 26;
float ratio = 1;

int piezoValueBefore;
int piezoValueAfter = 0;

int timeBefore;
int timeAfter = 0;

int timeGateBefore;
int timeGateAfter = 0;

int timeDifference;

float toRemove;

int piezoThreshold = 0;

byte gating = LOW;

int filter(int sample, float EMA_a)
{
	EMA_S = (EMA_a * sample) + ((1 - EMA_a) * EMA_S);
	highpass = sample - EMA_S;
	return highpass;
}

void ratioGate()
{
	if (gating == HIGH)
	{
		timeGateAfter = millis();
		timeDifference = timeGateAfter - timeGateBefore;
		if (timeDifference < 700)
		{
			toRemove = (float)timeDifference / 900;
			ratio = (float)1 - toRemove;
		}
		else
		{
			ratio = 0.20;
			gating = LOW;
		}
	}
}

void piezoHit()
{
	timeGateBefore = millis();
	gating = HIGH;
}

void readPiezo()
{
	piezoValueBefore = filter(analogRead(piezoPin), 0.04);
	if (piezoValueBefore < 0)
	{
		piezoValueBefore = 0;
	}
	piezoValueBefore = filter(piezoValueBefore, 0.04);
	if (piezoValueBefore < 0)
	{
		piezoValueBefore = 0;
	}
	if ((piezoValueBefore > 0) && (piezoValueAfter == 0) && (piezoValueBefore > piezoThreshold))
	{
		timeBefore = millis();
		if (timeBefore - timeAfter > 40)
		{
			piezoHit();
			timeAfter = timeBefore;
			Serial.println(piezoValueBefore);
		}
		timeAfter = timeBefore;
	}
	piezoValueAfter = piezoValueBefore;
}

void setup()
{
	Serial.begin(115200);
	esp_err_t err;

	// The I2S config as per the example
	const i2s_config_t i2s_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX), // Receive, not transfer
		.sample_rate = 44100,											 // 16KHz
		.bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,					 // could only get it to work with 32bits
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,					 // use right channel
		.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
		.dma_buf_count = 8,						  // number of buffers
		.dma_buf_len = 256,
	};

	// The pin config as per the setup
	const i2s_pin_config_t pin_config = {
		.bck_io_num = 19,	// Serial Clock (SCK)
		.ws_io_num = 5,		// Word Select (WS)
		.data_out_num = 18, // not used (only for speakers)
		.data_in_num = 21	// Serial Data (SD)
	};

	// Configuring the I2S driver and pins.
	// This function must be called before any I2S driver read/write operations.
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
	readPiezo();

	ratioGate();

	//read 256 samples (128 stereo samples)
	esp_err_t rxfb = i2s_read(I2S_NUM_0, &rxbuf[0], 128 * 4, &readsize, 1000);
	if (rxfb == ESP_OK && readsize == 128 * 4)
	{

		//extract stereo samples to mono buffers
		int y = 0;
		for (int i = 0; i < 128; i = i + 2)
		{
			l_in[y] = (float)rxbuf[i];
			r_in[y] = (float)rxbuf[i + 1];
			y++;
		}

		for (int i = 0; i < 64; i++)
		{

			l_out[i] = l_in[i] * ratio;
			r_out[i] = r_in[i] * ratio;
		}

		//merge two l and r buffers into a mixed buffer and write back to HW
		y = 0;
		for (int i = 0; i < 64; i++)
		{
			txbuf[y] = (int)l_out[i];
			txbuf[y + 1] = (int)r_out[i];
			y = y + 2;
		}

		i2s_write(I2S_NUM_0, &txbuf[0], 128 * 4, &readsize, 1000);
	}
}