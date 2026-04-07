#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ff.h"
#include "sd_card.h"
#include "hardware/gpio.h"
#include "cJSON.h"

typedef struct {
    uint8_t r, g, b;
} RGB;

void IO_main();

int SD_setup(FATFS *, FIL *, FRESULT *, UINT *);
int SD_read(FATFS *, FIL *, FRESULT *, UINT *, char *buf, size_t bufsize);


#endif