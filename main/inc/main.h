#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "ILS_MONITOR.h"
#include "IO_HANDLER.h"
#include "printer.h"

#include "types.h"
#include "SD_conf.h"

// main.h - add this line:
extern wiz_NetInfo g_net_info;

#endif