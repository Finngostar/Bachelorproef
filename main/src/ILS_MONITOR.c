/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include "ILS_MONITOR.h"
/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
//#define PLL_SYS_KHZ (133 * 1000)
//
///* Buffer */
//#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
//
///* Socket */
//#define SOCKET_MQTT 0
//
///* Port */
//#define PORT_MQTT 1883
//
///* Timeout */
//#define DEFAULT_TIMEOUT 1000 // 1 second
//
///* MQTT */
//#define MQTT_CLIENT_ID "rpi-pico"
//#define MQTT_USERNAME "wiznet"
//#define MQTT_PASSWORD "0123456789"
//#define MQTT_KEEP_ALIVE 60 // 60 milliseconds

char *MQTT_SUBSCRIBE_TOPICS[]  = {
    "cirs/ebbr/ibl/status_update/lines",
    "cirs/ebbr/ibl/status_update/wifi",
    "cirs/ebbr/ibl/status_update/lora",
    "cirs/ebbr/ibm/status_update/lines",
    "cirs/ebbr/ibm/status_update/wifi",
    "cirs/ebbr/ibm/status_update/lora",
    "cirs/ebbr/ibr/status_update/lines",
    "cirs/ebbr/ibr/status_update/wifi",
    "cirs/ebbr/ibr/status_update/lora",
    "cirs/ebbr/ibx/status_update/lines",
    "cirs/ebbr/ibx/status_update/wifi",
    "cirs/ebbr/ibx/status_update/lora",
    NULL
};

extern RGB LED_VALS_LINES[6];
extern RGB LED_VALS_WIFI[6];
extern RGB LED_VALS_LORA[6];

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 0, 101},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 0, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

/* MQTT */
static uint8_t g_mqtt_send_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_mqtt_recv_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_mqtt_broker_ip[4] = {192, 168, 0, 100};
static Network g_mqtt_network;
static MQTTClient g_mqtt_client;
static MQTTPacket_connectData g_mqtt_packet_connect_data = MQTTPacket_connectData_initializer;

/* Timer  */
static void repeating_timer_callback(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

/* MQTT */
static void message_arrived(MessageData *msg_data);
void subscribe_to_topics(char **);
void MQTT_parse_JSON(MQTTMessage*, MQTTString *);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
void MQTT_main()
{
    /* Initialize */
    int32_t retval = 0;

    set_clock_khz();
    printInfo("WIZchip SPI Init\n");
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    wizchip_1ms_timer_initialize(repeating_timer_callback);

    printInfo("Network Init\n");
    network_initialize(g_net_info);

    /* Get network information */
    print_network_information(g_net_info);

    NewNetwork(&g_mqtt_network, SOCKET_MQTT);

    retval = ConnectNetwork(&g_mqtt_network, g_mqtt_broker_ip, PORT_MQTT);

    if (retval != 1)
    {
        printError(" Network connect failed - exiting\n");

        while (1)
            ;
    }

    /* Initialize MQTT client */
    MQTTClientInit(&g_mqtt_client, &g_mqtt_network, DEFAULT_TIMEOUT, g_mqtt_send_buf, ETHERNET_BUF_MAX_SIZE, g_mqtt_recv_buf, ETHERNET_BUF_MAX_SIZE);

    /* Connect to the MQTT broker */
    g_mqtt_packet_connect_data.MQTTVersion = 3;
    g_mqtt_packet_connect_data.cleansession = 1;
    g_mqtt_packet_connect_data.willFlag = 0;
    g_mqtt_packet_connect_data.keepAliveInterval = MQTT_KEEP_ALIVE;
    g_mqtt_packet_connect_data.clientID.cstring = MQTT_CLIENT_ID;
    g_mqtt_packet_connect_data.username.cstring = MQTT_USERNAME;
    g_mqtt_packet_connect_data.password.cstring = MQTT_PASSWORD;

    retval = MQTTConnect(&g_mqtt_client, &g_mqtt_packet_connect_data);

    if (retval < 0)
    {
        printError("MQTT connect failed\n");

        while (1)
            ;
    }

    printInfo("MQTT connected\n");

    /* Subscribe */
    subscribe_to_topics(MQTT_SUBSCRIBE_TOPICS);

    //printf(" Subscribed\n");

    /* Infinite loop */
    while (1)
    {
        if ((retval = MQTTYield(&g_mqtt_client, 10)) < 0)
        {
            printf(" Yield error : %d\n", retval);

            while (1)
                ;
        }
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

/* MQTT */
static void message_arrived(MessageData *msg_data)
{
    MQTTMessage *message = msg_data->message;
    MQTTString *topic = msg_data->topicName;
    MQTT_parse_JSON(message, topic);
    //printf("%.*s\n", (uint32_t)message->payloadlen, (uint8_t *)message->payload);
}

/* Timer */
static void repeating_timer_callback(void)
{
    MilliTimer_Handler();
}

void subscribe_to_topics(char ** topics){
    for (int i = 0; topics[i] != NULL; i++) {
        int retval = MQTTSubscribe(&g_mqtt_client, topics[i], QOS0, message_arrived);

        if (retval < 0)
        {
            printf(" Subscribe failed : %d\n", retval);

            while (1)   
                ;
        }
        else {
            printf("[INFO] Subscribed to %s\n", topics[i]);
        }
    }
    printf("\n\n");
}

void MQTT_parse_JSON(MQTTMessage *message, MQTTString *topic) {
    if (topic->cstring != NULL) {
        printf("TOPIC: %s\n", topic->cstring);
    } else if (topic->lenstring.data != NULL) {
        printf("TOPIC: %.*s\n", topic->lenstring.len, topic->lenstring.data);
    } else {
        printf("TOPIC: (unknown)\n");
    }

    printf("************************************\n");
    // Create a null-terminated copy on stack (max 2KB based on your buffer size)
    char payload_str[ETHERNET_BUF_MAX_SIZE];
    
    if (message->payloadlen >= ETHERNET_BUF_MAX_SIZE) {
        printf("Error: Payload too large\n");
        return;
    }
    
    memcpy(payload_str, message->payload, message->payloadlen);
    payload_str[message->payloadlen] = '\0';  // Add null terminator
    
    // DEBUG: Print the raw payload
    /*
    printf("[DEBUG] Payload length: %d\n", message->payloadlen);
    printf("[DEBUG] Raw payload: %s\n", payload_str);
    printf("[DEBUG] Hex dump: ");
    for (int i = 0; i < message->payloadlen && i < 50; i++) {
        printf("%02X ", (unsigned char)payload_str[i]);
    }
    printf("\n");*/
    
    cJSON *json = cJSON_Parse(payload_str);
    
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error parsing JSON before: %s\n", error_ptr);
        } else {
            printf("Error parsing JSON (unknown)\n");
        }
        return;
    }

    cJSON *DME = cJSON_GetObjectItem(json, "dme");
    cJSON *VOR = cJSON_GetObjectItem(json, "vor");
    cJSON *GP = cJSON_GetObjectItem(json, "gp");
    cJSON *LLZ = cJSON_GetObjectItem(json, "llz");
    cJSON *FFM = cJSON_GetObjectItem(json, "ffm");
    cJSON *MM = cJSON_GetObjectItem(json, "mm");
    
    if(cJSON_IsNumber(DME)) {
        printf("DME: %d,\t", DME->valueint);
        assignLedVal(dme, DME->valueint);
    }
    if(cJSON_IsNumber(VOR)) {
        printf("VOR: %d,\t", VOR->valueint);
        assignLedVal(om, VOR->valueint);
    }
    if(cJSON_IsNumber(GP)) {
        printf("GP: %d,\t", GP->valueint);
        assignLedVal(gp, GP->valueint);
    }
    if(cJSON_IsNumber(LLZ)) {
        printf("LLZ: %d,\t", LLZ->valueint);
        assignLedVal(llz, LLZ->valueint);
    }
    if(cJSON_IsNumber(FFM)) {
        printf("FFM: %d,\t", FFM->valueint);
        assignLedVal(ffm, FFM->valueint);
    }
    if(cJSON_IsNumber(MM)) {
        printf("MM: %d,\t", MM->valueint);
        assignLedVal(mm, MM->valueint);
    }

    printf("\n********************************************\n\n");
    cJSON_Delete(json);
}

void assignLedVal(int type, int Status) {
    RGB colour;
    switch (type) {
        case llz:
            colour = determineLedColor(Status);
            LED_VALS_LINES[llz] = colour;
            break;
        case gp:
            colour = determineLedColor(Status);
            LED_VALS_LINES[gp] = colour;
            break;
        case dme:
            colour = determineLedColor(Status);
            LED_VALS_LINES[dme] = colour;
            break;
        case ffm:
            colour = determineLedColor(Status);
            LED_VALS_LINES[ffm] = colour;
            break;
        case mm:
            colour = determineLedColor(Status);
            LED_VALS_LINES[mm] = colour;
            break;
        case om:
            colour = determineLedColor(Status);
            LED_VALS_LINES[om] = colour;
            break;
        default:
            break;
    }
}

RGB determineLedColor(int status) {
    RGB colour;
    switch (status) {
        case 0:
            colour.r = 0;
            colour.g = 64;
            colour.b = 0;
            return colour;
        
        case 1:
            colour.r = 32;
            colour.g = 32;
            colour.b = 0;
            return colour;

        case 2:
            colour.r = 64;
            colour.g = 0;
            colour.b = 0;
            return colour;

        default:
            colour.r = 0;
            colour.g = 0;
            colour.b = 0;
            return colour;
    }

    return colour;
}