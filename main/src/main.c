#include "main.h"

RGB LED_VALS_LINES[6];
RGB LED_VALS_WIFI[6];
RGB LED_VALS_LORA[6];


wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 0, 101},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 0, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

FATFS fs;
FIL fil;
FRESULT fr;
UINT br;

char buf[512];

int main() {
    stdio_init_all();
    sleep_ms(3000);

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\033[31mPRESS 'b' within 3 seconds to enter bootloader...\033[m\n");
    absolute_time_t deadline = make_timeout_time_ms(3000);
    while(!time_reached(deadline)) {
        int c = getchar_timeout_us(0);
        if (c == 'b') {
            printf("Entering Bootloader...\n");
            reset_usb_boot(0, 0);
        }
    }
    

    printf("\033[31m*********************************\033[m\n");
    printf("\033[31mDUAL CORE IMPLEMENTATION:\033[m\n");
    printf("\033[36m\tCORE 0:\tIO - SD-card - LED's\033[m\n");
    printf("\033[36m\tCORE 1:\tMQTT-parsing\033[m\n");
    printf("\033[31m*********************************\033[m\n\n");

    printInfo("Loading SD configuration\n");
    SD_setup(&fs, &fil, &fr, &br);
    SD_read(&fs, &fil, &fr, &br, buf, sizeof(buf), &g_net_info);
    printInfo("CORE 1 BOOTUP\n");

    multicore_launch_core1(MQTT_main);

    printInfo("CORE 1 Launched\n");

    IO_main();

    while(1) {
        tight_loop_contents();
    }
}