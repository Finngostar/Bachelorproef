#ifndef IO_HANDLER_H
#define IO_HANDLER_H

#include "main.h"
#include "ILS_MONITOR.h"
#include "types.h"
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

void IO_main();

int SD_setup(FATFS *, FIL *, FRESULT *, UINT *);
int SD_read(FATFS *, FIL *, FRESULT *, UINT *, char *buf, size_t bufsize, wiz_NetInfo *net_info);

static void parse_ip(const char *str, uint8_t out[4]);
static void parse_mac(const char *str, uint8_t out[6]);

#endif