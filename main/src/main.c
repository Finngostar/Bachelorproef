#include "main.h"

RGB LED_VALS_LINES[6];
RGB LED_VALS_WIFI[6];
RGB LED_VALS_LORA[6];

int main() {
    stdio_init_all();
    sleep_ms(3000);

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\033[31m*********************************\033[m\n");
    printf("\033[31mDUAL CORE IMPLEMENTATION:\033[m\n");
    printf("\033[36m\tCORE 0:\tIO - SD-card - LED's\033[m\n");
    printf("\033[36m\tCORE 1:\tMQTT-parsing\033[m\n");
    printf("\033[31m*********************************\033[m\n\n");
    printInfo("CORE 1 BOOTUP\n");

    multicore_launch_core1(MQTT_main);

    printInfo("CORE 1 Launched\n");

    IO_main();

    while(1) {
        tight_loop_contents();
    }
}