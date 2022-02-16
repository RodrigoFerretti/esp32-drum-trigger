#include "Arduino.h"
#include "driver/i2s.h"
#include <driver/ledc.h>

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
    int mck_pin;
    int rx_buffer[64];
    int tx_buffer[64];
    int size_to_read;
    size_t size_read;
};

i2s_t i2s = {
    .port = I2S_NUM_0,
    .status = ESP_OK,
    .bit_depth = I2S_BITS_PER_SAMPLE_32BIT,
    .sample_rate = 44100,
    .dma_buff_count = 6,
    .dma_buff_len = 128,
    .dout_pin = 18,
    .din_pin = 21,
    .bck_pin = 19,
    .lck_pin = 5,
    .mck_pin = 15,
    .rx_buffer = {0},
    .tx_buffer = {0},
    .size_to_read = 64 * 4,
    .size_read = 0,
};

struct piezo_t
{
    int sample;
    int pin;
};

piezo_t piezo_left = {
    .sample = 0,
    .pin = 33,
};

piezo_t piezo_right = {
    .sample = 0,
    .pin = 34,
};

struct gate_t
{
    int threshold;
    int peak_sample;
    int loop_count;
    int start_scan_time;
    int elapsed_scan_time;
    int max_scan_time;
    int start_active_time;
    int elapsed_active_time;
    float release;
    bool is_active;
    float range;
    float range_multiplier;
    float apply_ratio;
};

gate_t gate_piezo_left = {
    .threshold = 100,
    .peak_sample = 0,
    .loop_count = 0,
    .start_scan_time = 0,
    .elapsed_scan_time = 0,
    .max_scan_time = 20,
    .start_active_time = 0,
    .elapsed_active_time = 0,
    .release = 444.0,
    .is_active = false,
    .range = 12,
    .range_multiplier = 0.0,
    .apply_ratio = 1,
};

gate_t gate_piezo_right = {
    .threshold = 100,
    .peak_sample = 0,
    .loop_count = 0,
    .start_scan_time = 0,
    .elapsed_scan_time = 0,
    .max_scan_time = 20,
    .start_active_time = 0,
    .elapsed_active_time = 0,
    .release = 444.0,
    .is_active = false,
    .range = 12,
    .range_multiplier = 0.0,
    .apply_ratio = 1,
};

struct high_pass_t
{
    int EMA_s;
    float EMA_a;
};

high_pass_t high_pass_piezo_left = {
    .EMA_s = 0,
    .EMA_a = 0.04,
};

high_pass_t high_pass_piezo_right = {
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
        .use_apll = true,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 11289600
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

    
    REG_WRITE(PIN_CTRL, 0xFF0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}

void setup()
{
    Serial.begin(115200);
    configure_i2s();
}

void apply_high_pass(high_pass_t *high_pass, int *sample)
{
    high_pass->EMA_s = (high_pass->EMA_a * *sample) + ((1 - high_pass->EMA_a) * high_pass->EMA_s);

    *sample = *sample - high_pass->EMA_s;
};

void apply_gate(gate_t *gate, int *scan_sample)
{
    if (*scan_sample > gate->threshold)
    {

        if (gate->loop_count == 0)
        {
            gate->start_scan_time = millis();
            gate->peak_sample = *scan_sample;
        }

        gate->elapsed_scan_time = millis() - gate->start_scan_time;

        if (gate->elapsed_scan_time < gate->max_scan_time)
        {

            if (*scan_sample > gate->peak_sample)
            {
                gate->peak_sample = *scan_sample;
            }

            gate->loop_count++;
        }

        else
        {
            gate->loop_count = 0;
            gate->range_multiplier = pow(0.5, gate->range / 6.0);
            gate->start_active_time = millis();
            gate->is_active = true;
        }
    }

    if (gate->is_active)
    {
        gate->elapsed_active_time = millis() - gate->start_active_time;

        if (gate->elapsed_active_time < gate->release)
        {
            float to_remove = (float)(gate->elapsed_active_time / (gate->release / (1 - gate->range_multiplier)));
            gate->apply_ratio = (float)1 - to_remove;
        }

        else
        {
            gate->apply_ratio = gate->range_multiplier;
            gate->is_active = false;
        }
    }
}

void stream_i2s()
{
    i2s.status = i2s_read(i2s.port, &i2s.rx_buffer[0], i2s.size_to_read, &i2s.size_read, portMAX_DELAY);

    if (i2s.status == ESP_OK && i2s.size_read == i2s.size_to_read)
    {
        for (int i = 0; i < 64; i = i + 2)
        {
            i2s.tx_buffer[i] = (int)i2s.rx_buffer[i] * gate_piezo_left.apply_ratio;
            i2s.tx_buffer[i + 1] = (int)i2s.rx_buffer[i + 1] * gate_piezo_left.apply_ratio;
        }

        i2s_write(i2s.port, &i2s.tx_buffer[0], i2s.size_to_read, &i2s.size_read, portMAX_DELAY);
    }
}

void loop()
{
    piezo_left.sample = analogRead(piezo_left.pin);
    apply_high_pass(&high_pass_piezo_left, &piezo_left.sample);
    apply_high_pass(&high_pass_piezo_left, &piezo_left.sample);
    apply_gate(&gate_piezo_left, &piezo_left.sample);

    piezo_right.sample = analogRead(piezo_right.pin);
    apply_high_pass(&high_pass_piezo_right, &piezo_right.sample);
    apply_high_pass(&high_pass_piezo_right, &piezo_right.sample);
    apply_gate(&gate_piezo_right, &piezo_right.sample);

    stream_i2s();
}