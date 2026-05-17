/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * avilib: https://github.com/lanyou1900/avilib.git
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload AVI file
 *   FFat/LittleFS:
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Copy files to SD card
 ******************************************************************************/
const char *root = "/root";
const char *sd = "/sd";
const char *avi_folder = "/avi";
// char *avi_filename = (char *)"/root/AviMp3Cinepak240p30fps.avi";

// Dev Device Pins: <https://github.com/moononournation/Dev_Device_Pins.git>

#include <Wire.h>
#include "es8311.h"
#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

// #include "PINS_T-DECK.h"
#include "PINS_ESP32_S3_LCD_1.47_CUSTOM.h"

size_t output_buf_size;
uint16_t *output_buf;

#include "AviFunc.h"

#define FILESYSTEM SD

// Make SD card uses another SPI channel (HSPI)
SPIClass SD_SPI(HSPI);


void setup()
{

// Device init. 
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  while(!Serial);
  Serial.println("AviCinepak");

  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, HIGH);
  // delay(10);
  // digitalWrite(TFT_RST, LOW);  // Pull Low to Reset
  // delay(100);
  // digitalWrite(TFT_RST, HIGH); // Back High to Run

  pinMode(TFT_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);
  
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // If display and SD shared same interface, init SPI first
  Serial.println("SD card supported");
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  Serial.println("SPI initialized");
  // Init Display
  // if (!gfx->begin())
  if (!gfx->begin(GFX_SPEED))
  {
    Serial.println("gfx->begin() display init failed!");
    gfx->fillScreen(RGB565_WHITE);
  }
  gfx->fillScreen(RGB565_BLACK);
  Serial.println("Screen initialized.");
// #ifdef GFX_BL
// #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
//   ledcSetup(0, 1000, 8);
//   ledcAttachPin(GFX_BL, 0);
//   ledcWrite(0, 204);
// #else  // ESP_ARDUINO_VERSION_MAJOR >= 3
//   ledcAttachChannel(GFX_BL, 1000, 8, 1);
//   ledcWrite(GFX_BL, 204);
// #endif // ESP_ARDUINO_VERSION_MAJOR >= 3
// #endif // GFX_BL

  // gfx->setTextColor(RGB565_WHITE, RGB565_BLACK);
  // gfx->setTextBound(60, 60, 240, 240);

// #if defined(SD_SUPPORTED)
  Serial.println("Using SD card as data source!");
  
  // SD.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
  if (!SD.begin(SD_CS, SD_SPI, 80000000, sd))
  {
    Serial.println("ERROR: File system mount failed!");
    return;
  }
  else
  {
    Serial.println("SD card mounted");
    output_buf_size = gfx->width() * gfx->height() * 2;
#if defined(RGB_PANEL) | defined(DSI_PANEL)
    output_buf = gfx->getFramebuffer();
#else
    output_buf = (uint16_t *)aligned_alloc(16, output_buf_size);
#endif
    if (!output_buf)
    {
      Serial.println("output_buf aligned_alloc failed!");
    }

    avi_init();
  }
}

void loop()
{
  // Serial.printf("Open folder: %s\n", avi_folder);
  // File dir = FILESYSTEM.open(avi_folder);
  Serial.printf("Open folder: %s\n", avi_folder);
  File dir = FILESYSTEM.open(avi_folder);
  if (!dir.isDirectory())
  {
    Serial.println("Target is not a directory");
    delay(5000); // avoid error repeat too fast
  }
  else {
    // target is a directory
    File file = dir.openNextFile();
    // Serial.println(file.path());
    while (file){
      if (!file.isDirectory()){
        std::string avi_filename = sd;
        avi_filename += file.path();

        // Serial.printf("filename 01 = %s\n",avi_filename.c_str());
        if ((avi_filename.rfind(".", 0) != 0) && ((int)avi_filename.find(".avi", 0) > 0)){
          // avi_filename = avi_folder;
          // avi_filename = file.path();

          Serial.printf("filename = %s\n", avi_filename.c_str());

          if (SD.exists(avi_filename.c_str()))
            Serial.println("Target video file exists!");


          if (avi_open((char *)avi_filename.c_str()))
          {
            // garbage detection
            if (avi_w <= 0 || avi_w > 480 || avi_h <= 0 || avi_h > 480 || avi_total_frames <= 0) {
                Serial.printf("FATAL: Corrupted AVI Header - W:%ld H:%ld Frames:%ld\n", avi_w, avi_h, avi_total_frames);
                avi_close();
                delay(5000); 
                return; 
            }




            Serial.println("AVI start");
            gfx->fillScreen(RGB565_BLACK);

            avi_start_ms = millis();

            Serial.println("Start play loop");
            while (avi_curr_frame < avi_total_frames)
            {
              if (avi_decode())
              {
                avi_draw(0, 0);
              }
            }

            avi_close();
            Serial.println("AVI end");

            // avi_show_stat();
          }
          else{
            // Playing failed
            break; // terminate 
          }
        }
      }
      else{
        Serial.println("file is a directory!");
      }
    }
  }







  // Original version
  // if (avi_open(avi_filename))
  // {
  //   Serial.println("AVI start");
  //   gfx->fillScreen(RGB565_BLACK);

  //   avi_start_ms = millis();

  //   Serial.println("Start play loop");
  //   while (avi_curr_frame < avi_total_frames)
  //   {
  //     if (avi_decode())
  //     {
  //       avi_draw(0, 0);
  //     }
  //   }

  //   avi_close();
  //   Serial.println("AVI end");

  //   avi_show_stat();
  // }

  delay(60 * 1000);
}
