#include "Arduino.h"
#include "driver/i2s.h"
#include <../modules/trigger.cpp>

const int BUFFER_SIZE = 64;

struct i2s_t
{
    i2s_port_t port;
    esp_err_t status;
    int sample_rate;
    int dma_buff_count;
    int dma_buff_len;
    int dout_pin;
    int din_pin;
    int bck_pin;
    int lck_pin;
    int size_to_read;
    size_t size_read;
    int rx_buffer[BUFFER_SIZE];
    int tx_buffer[BUFFER_SIZE];
    ;

    void setup()
    {
        const i2s_config_t i2s_config = {
            .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
            .sample_rate = sample_rate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = dma_buff_count,
            .dma_buf_len = dma_buff_len,
            .use_apll = true,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 11289600,
        };

        const i2s_pin_config_t pin_config = {
            .bck_io_num = bck_pin,
            .ws_io_num = lck_pin,
            .data_out_num = dout_pin,
            .data_in_num = din_pin,
        };

        status = i2s_driver_install(port, &i2s_config, 0, NULL);

        if (status != ESP_OK)
        {
            Serial.printf("Failed installing driver: %d\n", status);
            while (true)
                ;
        }

        status = i2s_set_pin(port, &pin_config);

        if (status != ESP_OK)
        {
            Serial.printf("Failed setting pin: %d\n", status);
            while (true)
                ;
        }

        Serial.println("I2S driver installed.");

        REG_WRITE(PIN_CTRL, 0xFF0);
        PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    }

    void stream()
    {
        status = i2s_read(port, &rx_buffer[0], size_to_read, &size_read, portMAX_DELAY);

        if (status != ESP_OK || size_read != size_to_read)

            return;

        for (int i = 0; i < BUFFER_SIZE; i = i + 2)
        {
            tx_buffer[i] = player.get_sample() * 32000;
            tx_buffer[i + 1] = tx_buffer[i];
        }

        i2s_write(port, &tx_buffer[0], size_to_read, &size_read, portMAX_DELAY);
    }
};

i2s_t i2s = {
    .port = I2S_NUM_0,
    .status = ESP_OK,
    .sample_rate = 44100,
    .dma_buff_count = 6,
    .dma_buff_len = 128,
    .dout_pin = 17,
    .din_pin = 18,
    .bck_pin = 5,
    .lck_pin = 16,
    .size_to_read = BUFFER_SIZE * sizeof(int32_t),
    .size_read = 0,
};
