/*******************************************************************************
 * JPEG Image Viewer
 * This is a simple JPEG image viewer example
 * Image Source: https://github.blog/2014-11-24-from-sticker-to-sculpture-the-making-of-the-octocat-figurine/
 *
 * Dependent libraries:
 * JPEGDEC: https://github.com/bitbank2/JPEGDEC.git
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload JPEG file
 *   FFat (ESP32):
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   LittleFS (ESP32 / ESP8266 / Pico):
 *     upload LittleFS data with ESP8266 LittleFS Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *     ESP8266: https://github.com/earlephilhower/arduino-esp8266littlefs-plugin
 *     Pico: https://github.com/earlephilhower/arduino-pico-littlefs-plugin.git
 *   SPIFFS (ESP32):
 *     upload SPIFFS data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Most Arduino system built-in support SD file system.
 *     Wio Terminal require extra dependant Libraries:
 *     - Seeed_Arduino_FS: https://github.com/Seeed-Studio/Seeed_Arduino_FS.git
 *     - Seeed_Arduino_SFUD: https://github.com/Seeed-Studio/Seeed_Arduino_SFUD.git
 ******************************************************************************/
#define JPEG_FILENAME "/output_240x240.jpg"
/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C2/3 various dev board: CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-C5 various dev board  : CS: 23, DC: 24, RST: 25, BL: 26, SCK: 10, MOSI:  8, MISO: nil
 * ESP32-C6 various dev board  : CS: 18, DC: 22, RST: 23, BL: 15, SCK: 21, MOSI: 19, MISO: nil
 * ESP32-H2 various dev board  : CS:  0, DC: 12, RST:  8, BL: 22, SCK: 10, MOSI: 25, MISO: nil
 * ESP32-P4 various dev board  : CS: 26, DC: 27, RST: 25, BL: 24, SCK: 36, MOSI: 32, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

// #define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
// #if defined(DISPLAY_DEV_KIT)
// Arduino_GFX *gfx = create_default_Arduino_GFX();
// #else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
// Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
// Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 3 /* rotation */, false /* IPS */);

// #endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
#include <Seeed_FS.h>
#include <SD/Seeed_SD.h>
#elif defined(TARGET_RP2040) || defined(PICO_RP2350)
#include <LittleFS.h>
#include <SD.h>
#elif defined(ESP32)
#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>
#elif defined(ESP8266)
#include <LittleFS.h>
#include <SD.h>
#else
#include <SD.h>
#endif

#include "JpegFunc.h"
#include "PINS_ESP32_S3_LCD_1.47_CUSTOM.h"

// pixel drawing callback
static int jpegDrawCallback(JPEGDRAW *pDraw)
{
  Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}

// variables for sequential play of images
// Dynamic storage allocations for filenames discovered during boots
String dynamic_image_list[SLIDESHOW_MAX_IMAGES];
int discovered_image_count = 0;
int current_image_index = 0;

// Scans the root directory of LittleFS for JPEG images
void scanRootDirectory()
{
  Serial.println(F("Scanning root directory (/) for JPEGs..."));
  discovered_image_count = 0;

  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println(F("Failed to open root directory!"));
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      String fileName = file.name();
      String filePath = file.path(); // Returns absolute path starting with '/'
      
      fileName.toLowerCase();
      // Verify if file extension is valid target format
      if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg")) {
        if (discovered_image_count < SLIDESHOW_MAX_IMAGES) {
          dynamic_image_list[discovered_image_count] = filePath;
          Serial.printf(" Found [%d]: %s\n", discovered_image_count + 1, filePath.c_str());
          discovered_image_count++;
        } else {
          Serial.printf("Reached max limit constraint of %d images. Skipping extra files.\n", SLIDESHOW_MAX_IMAGES);
          break;
        }
      }
    }
    file = root.openNextFile();
  }
  
  Serial.printf("Scan finished. Total images found: %d\n\n", discovered_image_count);
}



void setup()
{
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  while(!Serial);
  Serial.println("Arduino_GFX JPEG Image Viewer example");
  
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);

  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Init Display
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() failed!");
    gfx->fillScreen(RGB565_WHITE);
  }
  gfx->fillScreen(RGB565_BLACK);

// #ifdef GFX_BL
  // pinMode(TFT_BL, OUTPUT);
  // digitalWrite(TFT_BL, HIGH);
// #endif

/* Wio Terminal */
#if defined(ARDUINO_ARCH_SAMD) && defined(SEEED_GROVE_UI_WIRELESS)
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL))
#elif defined(TARGET_RP2040) || defined(PICO_RP2350)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#elif defined(ESP32)
  Serial.println("System is ESP32");
  // if (!FFat.begin())
  if (!LittleFS.begin())
  // if (!SPIFFS.begin())
  // SPI.begin(12 /* CLK */, 13 /* D0/MISO */, 11 /* CMD/MOSI */);
  // if (!SD.begin(10 /* CS */, SPI))
  // pinMode(10 /* CS */, OUTPUT);
  // digitalWrite(10 /* CS */, HIGH);
  // SD_MMC.setPins(12 /* CLK */, 11 /* CMD/MOSI */, 13 /* D0/MISO */);
  // if (!SD_MMC.begin("/root", true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_DEFAULT))
  // SD_MMC.setPins(12 /* CLK */, 11 /* CMD/MOSI */, 13 /* D0/MISO */, 14 /* D1 */, 15 /* D2 */, 10 /* D3/CS */);
  // if (!SD_MMC.begin("/root", false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
#elif defined(ESP8266)
  if (!LittleFS.begin())
  // if (!SD.begin(SS))
#else
  if (!SD.begin())
#endif
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
  }
  else
  {
    Serial.println(F("File System Mounted"));
    unsigned long start = millis();

    Serial.printf("gfx->width() = %i, gfx->height() = %i\n",gfx->width(),gfx->height());

    // gfx->fillScreen(RGB565_GREEN); // for testing 

    // Scan root path and build directory manifest indexes
    scanRootDirectory();

    if (discovered_image_count == 0) {
      Serial.println(F("Warning: No viable JPEG files found in root storage partition."));
      gfx->println(F("No images found in root (/)."));
    }

    // jpegDraw(JPEG_FILENAME, jpegDrawCallback, true /* useBigEndian */,
    //          0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
    // Serial.printf("Time used: %lu\n", millis() - start);
  }

  delay(5000);
}

void loop()
{
  // Fallback state context handler if partition directories are empty
  if (discovered_image_count == 0) {
    delay(2000);
    Serial.println("No image found in /, scanning again");
    scanRootDirectory(); // Re-scan periodically to see if files were uploaded via USB CDC
    return;
  }

  // get the path of an image
  String current_file_path = dynamic_image_list[current_image_index];
  Serial.printf("Displaying [%d/%d]: %s\n", current_image_index + 1, discovered_image_count, current_file_path.c_str());

  unsigned long start_time = millis();

  // int w = gfx->width();
  // int h = gfx->height();

  // display the jpeg
  jpegDraw(current_file_path.c_str(), jpegDrawCallback, true /* useBigEndian */,
           0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
  

  // Extra: shows the image in random location 
  // jpegDraw(current_file_path.c_str(), jpegDrawCallback, true /* useBigEndian */,
  //          random(w * 2) - w /* x */,
  //          random(h * 2) - h /* y */,
  //          w /* widthLimit */, h /* heightLimit */);

  Serial.printf("Time used: %lu ms\n", millis() - start_time);

  // Index step execution loops back to 0 when bounded limits are hit
  current_image_index++;
  if (current_image_index >= discovered_image_count) {
    current_image_index = 0;
  }

  // Enforce wait times configured inside the shared library header definitions
  delay(SWITCH_INTERVAL_MS);
}
