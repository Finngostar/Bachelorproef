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

extern wiz_NetInfo g_net_info;

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

    // Replace with this:
    pio = pio0;
    offset = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    bool succes = true; // pio_claim_unused_sm panics on failure, so if we get here it succeeded
    hard_assert(succes);

    //ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    //SD_setup(&fs, &fil, &fr, &br);
    //SD_read(&fs, &fil, &fr, &br, buf, sizeof(buf));

    while(1) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            put_pixel(pio, sm, urgb_u32(LED_VALS_LINES[i].r,
                                        LED_VALS_LINES[i].g,
                                        LED_VALS_LINES[i].b));
        }

        sleep_us(100);
    }
}