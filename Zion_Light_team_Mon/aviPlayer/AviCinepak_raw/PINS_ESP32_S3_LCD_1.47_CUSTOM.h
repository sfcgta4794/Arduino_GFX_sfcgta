#pragma once

// Check if selected right Board
// #if !defined(ESP32) || !defined(CONFIG_IDF_TARGET_ESP32C6)
// #error "Please select Board: ESP32C6"
// #endif
// // Check if enabled USB CDC On Boot
// #if !ARDUINO_USB_CDC_ON_BOOT
// #error "Please select USB CDC On Boot: Enabled"
// #endif

#define DEV_DEVICE_PINS

/*
 * 1. PIN DEFINITIONS
 * Mapped according to your ESP32-S3-DEV_KIT-N16R8 setup.
 */
#define TFT_BL          21    // BLK
#define TFT_CS          13    // CS
#define TFT_DC          9     // DC
#define TFT_RST         10    // RES
#define TFT_MOSI        11    // SDA
#define TFT_SCLK        12    // SCL
#define TFT_MISO        -1    // Not connected

/*
 * 2. DISPLAY SPECIFICATIONS
 * Specific to ZJY147S0800TG01 (1.47" 172x320)
 */
#define TFT_WIDTH       240
#define TFT_HEIGHT      240
#define TFT_OFFSET_X    0   // Required to center the 172px width in 240px memory
#define TFT_OFFSET_Y    0
#define GFX_SPEED       80000000UL // 80MHz SPI speed for the S3 （when using internal memory)

/*
 * 3. GFX INSTANTIATION
 * Using the Arduino_GFX library classes.
 */
#define GFX_SUPPORTED
#include <Arduino_GFX_Library.h>

// Initialize the ESP32-S3 SPI Bus
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    TFT_DC, 
    TFT_CS,
    // -1, -1, -1, HSPI
    TFT_SCLK, 
    TFT_MOSI, 
    TFT_MISO
);

// Initialize the ST7789 Driver
Arduino_ST7789 *gfx = new Arduino_ST7789(
    bus, 
    TFT_RST, 
    1,               /* rotation: 0 (portrait) */
    true,            /* IPS: true for ZJY model */
    TFT_WIDTH, 
    TFT_HEIGHT,
    TFT_OFFSET_X, TFT_OFFSET_Y, /* col/row offset 1 */
    TFT_OFFSET_X, TFT_OFFSET_Y  /* col/row offset 2 */
);

#define GFX_BL          TFT_BL

/*
 * 4. OPTIONAL: SD CARD (Shared SPI Bus)
 * If you add an SD card later for AVI playback, use these pins.
 */
// #define SD_SUPPORTED
// #define SD_SCK          TFT_SCLK
// #define SD_MOSI         TFT_MOSI
// #define SD_MISO         14    // Example MISO pin for SD card
// #define SD_CS           15    // Example CS pin for SD card