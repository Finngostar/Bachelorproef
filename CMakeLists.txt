# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH ${CMAKE_SOURCE_DIR}/libraries/pico-sdk)
    message(STATUS "PICO_SDK_PATH = ${PICO_SDK_PATH}")
endif()

include(rp2040_hat_c-patch.cmake)
include(pico_sdk_import.cmake)
include(rp2040_hat_c_sdk_version.cmake)

set(PROJECT_NAME RP2040-HAT-C)
project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(WIZNET_CHIP W5500)
if(${WIZNET_CHIP} STREQUAL W5100S)
    add_definitions(-D_WIZCHIP_=W5100S)
elseif(${WIZNET_CHIP} STREQUAL W5500)
    add_definitions(-D_WIZCHIP_=W5500)
else()
    message(FATAL_ERROR "WIZNET_CHIP is wrong = ${WIZNET_CHIP}")
endif()
message(STATUS "WIZNET_CHIP = ${WIZNET_CHIP}")

if(NOT DEFINED WIZNET_DIR)
    set(WIZNET_DIR ${CMAKE_SOURCE_DIR}/libraries/ioLibrary_Driver)
    message(STATUS "WIZNET_DIR = ${WIZNET_DIR}")
endif()
if(NOT DEFINED MBEDTLS_DIR)
    set(MBEDTLS_DIR ${CMAKE_SOURCE_DIR}/libraries/mbedtls)
    message(STATUS "MBEDTLS_DIR = ${MBEDTLS_DIR}")
endif()
if(NOT DEFINED PORT_DIR)
    set(PORT_DIR ${CMAKE_SOURCE_DIR}/port)
    message(STATUS "PORT_DIR = ${PORT_DIR}")
endif()

set(ENABLE_PROGRAMS OFF CACHE BOOL "Build mbedtls programs")
set(ENABLE_TESTING OFF CACHE BOOL "Build mbedtls testing")
add_definitions(-DMBEDTLS_CONFIG_FILE="${PORT_DIR}/mbedtls/inc/ssl_config.h")
add_definitions(-DSET_TRUSTED_CERT_IN_SAMPLES)

add_subdirectory(main/src)          # <-- your renamed example folder

add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src fatfs_build)
add_subdirectory(${MBEDTLS_DIR})
add_subdirectory(${PORT_DIR})

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(ENABLE_CJSON_TEST OFF CACHE BOOL "Enable cJSON tests" FORCE)
set(ENABLE_CJSON_UTILS OFF CACHE BOOL "Enable cJSON utils" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/cjson)

add_compile_options(
    -Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
)