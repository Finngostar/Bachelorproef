#ifndef ILS_MONITOR_H
#define ILS_MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types_X.h"
#include "port_common.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "mqtt_interface.h"
#include "MQTTClient.h"
#include "timer.h"
#include <time.h>   
#include "cJSON.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "IO_HANDLER.h"
#include "printer.h"

#define PLL_SYS_KHZ (133 * 1000)
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
#define SOCKET_MQTT 0
#define PORT_MQTT 1883
#define DEFAULT_TIMEOUT 1000

#define MQTT_CLIENT_ID "rpi-pico"
#define MQTT_USERNAME "wiznet"
#define MQTT_PASSWORD "0123456789"
#define MQTT_KEEP_ALIVE 60

extern char *MQTT_SUBSCRIBE_TOPICS[];

enum {
    llz = 0,
    gp,
    dme,
    ffm,
    mm,
    om
};

void MQTT_main();
void subscribe_to_topics(char **topics);
void MQTT_parse_JSON(MQTTMessage *message, MQTTString *topic);
void assignLedVal(int typ, int Status);
RGB determineLedColor(int status);

#endif