#include "hw_config.h"
#include "sd_card.h"
#include "my_spi.h"

static spi_t spi = {
    .hw_inst   = spi1,   // switch to SPI1
    .miso_gpio = 12,
    .mosi_gpio = 15,
    .sck_gpio  = 14,
    .baud_rate = 12500 * 1000,
    .spi_mode  = 0,
};

static sd_spi_if_t spi_if = {
    .spi     = &spi,
    .ss_gpio = 13,
};

// SD card descriptor
static sd_card_t sd_card = {
    .type     = SD_IF_SPI,
    .spi_if_p = &spi_if,
};

size_t sd_get_num() { return 1; }

sd_card_t *sd_get_by_num(size_t num) {
    if (num == 0) return &sd_card;
    return NULL;
}