set(TARGET_NAME main)

add_executable(${TARGET_NAME}
    ${TARGET_NAME}.c
    IO_HANDLER.c
    ILS_MONITOR.c
    printer.c
    hw_config.c
)

target_include_directories(${TARGET_NAME} PRIVATE ../inc
    ../../libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include
    ../../libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source
    ../../libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver
    ../../libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SPI
)

pico_generate_pio_header(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_link_libraries(${TARGET_NAME} PRIVATE
    pico_stdlib
    hardware_spi
    hardware_dma
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    ETHERNET_FILES
    IOLIBRARY_FILES
    MQTT_FILES
    TIMER_FILES
    cjson
    pico_multicore
    hardware_pio
)

pico_enable_stdio_usb(${TARGET_NAME} 1)
pico_enable_stdio_uart(${TARGET_NAME} 0)
pico_add_extra_outputs(${TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE
    MAX_MESSAGE_HANDLERS=13
)