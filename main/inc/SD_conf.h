#ifndef SD_CONF_H
#define SD_CONF_H

#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "sd_card.h"
#include "cJSON.h"
#include "wizchip_conf.h"

int SD_setup(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br);
int SD_read(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br, char *buf, size_t bufsize, wiz_NetInfo *net_info);

static void parse_ip(const char *str, uint8_t out[4]);
static void parse_mac(const char *str, uint8_t out[6]);

#endif