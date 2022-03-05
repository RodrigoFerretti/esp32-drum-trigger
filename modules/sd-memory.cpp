#include <../modules/sample-loader.cpp>

struct sd_t
{
    int sck_pin;
    int miso_pin;
    int mosi_pin;
    int cs_pin;

    void setup()
    {
        SPIClass *spi = NULL;
        spi = new SPIClass(HSPI);
        spi->begin(sck_pin, miso_pin, mosi_pin, cs_pin);
        SD.begin(cs_pin, *spi);
    }
};

sd_t sd = {
    .sck_pin = 14,
    .miso_pin = 27,
    .mosi_pin = 26,
    .cs_pin = 21,
};
