#include "IO_HANDLER.h"

#define IS_RGBW false
#define NUM_PIXELS 6

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
#define WS2812_PIN 2
#endif

#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

extern RGB LED_VALS_LINES[6];
extern RGB LED_VALS_WIFI[6];
extern RGB LED_VALS_LORA[6];

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
        ((uint32_t) (r) << 8) |
        ((uint32_t) (g) << 16) |
        (uint32_t) (b);
}

void IO_main() {
    PIO pio;
    uint sm;
    uint offset;

    FATFS fs;
    FIL fil;
    FRESULT fr;
    UINT br;

    char buf[512];

    // Replace with this:
    pio = pio0;
    offset = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    bool succes = true; // pio_claim_unused_sm panics on failure, so if we get here it succeeded
    hard_assert(succes);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    SD_setup(&fs, &fil, &fr, &br);
    SD_read(&fs, &fil, &fr, &br, buf, sizeof(buf));

    while(1) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(LED_VALS_LINES[i].r,
                                        LED_VALS_LINES[i].g,
                                        LED_VALS_LINES[i].b));
        }

        sleep_us(100);
    }
}

int SD_setup(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br) {

    printf("\033[32m[INFO]: Mounting SD-card\033[0m\n");

    *fr = f_mount(fs, "", 1);

    if (*fr != FR_OK) {
        printf("\033[31m[ERROR]: f_mount failed (%d)\033[0m\n", *fr);
        return 1;
    } else {
        printf("\033[32m[INFO]: Mounted SD-card\033[0m\n");
    }

    return 0;

}

int SD_read(FATFS *fs, FIL *fil, FRESULT *fr, UINT *br, char *buf, size_t bufsize) {

    // --- READ FILE ---
    *fr = f_open(fil, "Config.json", FA_READ);
    if (*fr != FR_OK) {
        printf("\033[31m[ERROR]: f_open failed (%d)\033[0m\n", *fr);
        return 1;
    } else {
        printf("\033[32m[INFO]: Opened File\033[0m\n");
    }
    printf("\n");

    *fr = f_read(fil, buf, bufsize - 1, br);
    buf[*br] = '\0';
    f_close(fil);

    printf("\033[31mRAW JSON: %s\033[0m\n", buf);

    // --- PARSE JSON ---
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL) {
        printf("ERROR: JSON Parse failed near; %s\n", cJSON_GetErrorPtr());
        return 1;
    }

    printf("\n\n");

    // --- EXTRACT VALUES ---
    cJSON *mac = cJSON_GetObjectItemCaseSensitive(json, "MAC-addr");
    cJSON *ip = cJSON_GetObjectItemCaseSensitive(json, "IP-addr");
    cJSON *gw = cJSON_GetObjectItemCaseSensitive(json, "gateway");
    cJSON *sn = cJSON_GetObjectItemCaseSensitive(json, "subnet");
    cJSON *dns = cJSON_GetObjectItemCaseSensitive(json, "DNS");

    if (mac) printf("MAC:\t\t%s\n", mac->valuestring);
    if (ip) printf("IP:\t\t%s\n", ip->valuestring);
    if (gw) printf("Gateway:\t%s\n", gw->valuestring);
    if (sn) printf("Subnet:\t\t%s\n", sn->valuestring);
    if (dns) printf("DNS:\t\t%s\n", dns->valuestring);

    printf("\n");

    // --- CLEANUP ---
    cJSON_Delete(json);
    f_unmount("");
    printf("[INFO]: unmounted SD-card\n");
    printf("[INFO]: DONE -- NOP-loop started\n\n");

    return 0;
}