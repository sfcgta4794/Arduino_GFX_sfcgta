
/*******************************************************************************
 * AVI Player example
 *
 * Dependent libraries:
 * Arduino_GFX: https://github.com/moononournation/Arduino_GFX.git
 * libhelix: https://github.com/pschatzmann/arduino-libhelix.git
 * ESP32_JPEG: https://github.com/esp-arduino-libs/ESP32_JPEG.git
 *
 * Setup steps:
 * 1. Change your LCD parameters in Arduino_GFX setting
 * 2. Upload AVI file
 *   FFat/LittleFS:
 *     upload FFat (FatFS) data with ESP32 Sketch Data Upload:
 *     ESP32: https://github.com/lorol/arduino-esp32fs-plugin
 *   SD:
 *     Copy files to SD card
 *
 * Video Format:
 * code "cvid": Cinepak
 * cod "MJPG": MJPEG
 *
 * Audio Format:
 * code 1: PCM
 * code 85: MP3
 ******************************************************************************/
// const char *root = "/root";
const char *avi_folder = "/avi";

// New code section


#include <Arduino_GFX_Library.h>

#include <LittleFS.h>
#include <FS.h>
#include <YCbCr2RGB.h>
#include <gfxfont.h>

// --- Hardware Pins ---
#define TFT_BL 21
#define TFT_DC 9
#define TFT_CS 13
#define TFT_SCLK 12
#define TFT_MOSI 11
#define TFT_RST 10

// --- Screen Specs (ZJY147S0800TG01) ---
#define TFT_WIDTH 172
#define TFT_HEIGHT 320
#define TFT_OFFSET_X 34
#define GFX_SPEED 80000000UL
#define BLACK 0x0000

// Setup Display
Arduino_DataBus *bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_ST7789 *gfx = new Arduino_ST7789(
    bus, TFT_RST, 0 /* Rotation */, true /* IPS */, 
    TFT_WIDTH, TFT_HEIGHT, 
    TFT_OFFSET_X, 0, TFT_OFFSET_X, 0
);

#include "AviFunc.h"

void setup() {
  Serial.begin(115200);
  // REQUIRED for USB CDC: Wait for the serial port to actually open
  unsigned long start_time = millis();
  while (!Serial && millis() - start_time < 5000) {
    delay(10);
  }
  Serial.println("\n--- AVI Player Initialization ---");
  
  // 1. Initialize Display
  if (!gfx->begin(GFX_SPEED)) {
    Serial.println("Display Init Failed!");
  }
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  // 3. Initialize Display
  if (!gfx->begin(GFX_SPEED)) {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

  // Ensure PSRAM is enabled in Tools menu for your N16R8
  output_buf_size = 172 * 320 * 2; // Width * Height * 2 bytes
  output_buf = (uint16_t *)heap_caps_malloc(output_buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  if (!output_buf) {
    Serial.println("PSRAM Allocation failed! Check Tools > PSRAM setting.");
  } else {
    Serial.println("PSRAM Allocation success");
  }

  // 2. Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed! Did you upload the data?");
    return;
  }
  if (!LittleFS.begin(false,"/root")){
    Serial.println("LittleFS Mount Failed.");
  }
  Serial.println("LittleFS Mounted Successfully.");

  

  // 3. Initialize AVI decoder
  avi_init();
}

void loop() {
  Serial.println("XXX");
  // // Look for AVI files in the /avi folder of LittleFS
  File root = LittleFS.open("/avi");
  if (!root || !root.isDirectory()) {
    Serial.println("Please create an /avi folder in your data directory.");
    delay(5000);
    return;
  }
  
  Serial.println("Trying to read documents");
  File file = root.openNextFile();

  // // TODO: check this logic to diagnose for reason which blocks the playing of music
  while (file) {
    Serial.printf("File name is %s\n", String(file.name()));
    String path = String(file.name());
    
    if (path.endsWith(".avi")) {
      String full_path = "/root/avi/" + path;
      // Serial.printf("Playing: %s\n", full_path);
      
      if (avi_open((char *)full_path.c_str())) {
        Serial.println("Playback finished.");
      } else {
        Serial.println("avi_open failed! Check codec (MJPEG) and path.");
      }
      // if (avi_open((char*)full_path.c_str())) {
      //   // while (avi_curr_frame < avi_total_frames) {
      //   //   if (avi_decode()) {
      //   //     // Draw frame at 0,0
      //   //     avi_draw(0, 0);
      //   //   }
      //   //   yield(); // Keep ESP32 watchdog happy
      //   // }
      //   // avi_close();
      //   Serial.println
      // }
    }
    file = root.openNextFile();
  }
  
  // Serial.println("Finished playing all videos. Restarting playlist...");
  delay(1000);
}




// Old code section


















// #include <Wire.h>
// #include "es8311.h"
// // Dev Device Pins: <https://github.com/moononournation/Dev_Device_Pins.git>
// // #include "PINS_AD35-S3.h"
// // #include "PINS_ESP32-S3-Touch-LCD-1_3.h"
// // #include "PINS_ESP32-S3-Touch-LCD-1_3_prism.h"
// // #include "PINS_ESP32-S3-Touch-AMOLED-1_8.h"
// // #include "PINS_ESP32-S3-Touch-LCD-1_46.h"
// // #include "PINS_ESP32-S3-Touch-LCD-2_8.h"
// // #include "PINS_IBUBLY.h"
// // #include "PINS_JC1060P470.h"
// // #include "PINS_JC3248W535.h"
// // #include "PINS_JC8012P4A1.h"
// // #include "PINS_T-DECK.h" # uncomment this to revert to original version. 
// // #include "PINS_T4_S3.h"

// // Use customized layout
// #include "PINS_ESP32_S3_LCD_1.47_CUSTOM.h"

// #ifdef I2S_OUTPUT
// #define AVI_SUPPORT_AUDIO
// #endif

// #include "AviFunc.h"

// #ifdef AVI_SUPPORT_AUDIO
// #include "esp32_audio.h"
// #endif

// #include <string>

// #include <FFat.h>
// #include <LittleFS.h>
// #include <SPIFFS.h>
// #include <SD.h>
// #ifdef SOC_SDMMC_HOST_SUPPORTED
// #include <SD_MMC.h>
// #endif

// void setup()
// {
// #ifdef DEV_DEVICE_INIT
//   DEV_DEVICE_INIT();
// #endif

//   Serial.begin(115200);
//   // Serial.setDebugOutput(true);
//   // while(!Serial);
//   Serial.println("AVI Player");

//   // If display and SD shared same interface, init SPI first
// #ifdef SPI_SCK
//   SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
// #endif

//   // Init Display
//   // if (!gfx->begin())
//   if (!gfx->begin(GFX_SPEED))
//   {
//     Serial.println("gfx->begin() failed!");
//   }
//   gfx->fillScreen(RGB565_BLACK);
// #if defined(RGB_PANEL) || defined(DSI_PANEL) || defined(CANVAS)
//   gfx->flush(true /* force_flush */);
// #endif

// #ifdef GFX_BL
// #if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR < 3)
//   ledcSetup(0, 1000, 8);
//   ledcAttachPin(GFX_BL, 0);
//   ledcWrite(0, 63);
// #else  // ESP_ARDUINO_VERSION_MAJOR >= 3
//   ledcAttachChannel(GFX_BL, 1000, 8, 1);
//   ledcWrite(GFX_BL, 63);
// #endif // ESP_ARDUINO_VERSION_MAJOR >= 3
// #endif // GFX_BL

//   // gfx->setTextColor(RGB565_WHITE, RGB565_BLACK);
//   // gfx->setTextBound(60, 60, 240, 240);

// #ifdef AVI_SUPPORT_AUDIO
// #ifdef AUDIO_EXTRA_PRE_INIT
//   AUDIO_EXTRA_PRE_INIT();
// #endif

//   i2s_init();

// #ifdef AUDIO_MUTE
//   pinMode(AUDIO_MUTE, OUTPUT);
//   digitalWrite(AUDIO_MUTE, LOW); // mute first
// #endif
// #endif // AVI_SUPPORT_AUDIO

// #if defined(SD_D1) && defined(SOC_SDMMC_HOST_SUPPORTED)
// #define FILESYSTEM SD_MMC
//   Serial.println("mount SD_MMC 4-bit");
//   SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */, SD_D1, SD_D2, SD_CS /* D3 */);
//   if (!SD_MMC.begin(root, false /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
// #elif defined(SD_SCK) && defined(SOC_SDMMC_HOST_SUPPORTED)
// #define FILESYSTEM SD_MMC
//   Serial.println("mount SD_MMC 1-bit");
//   pinMode(SD_CS, OUTPUT);
//   digitalWrite(SD_CS, HIGH);
//   SD_MMC.setPins(SD_SCK, SD_MOSI /* CMD */, SD_MISO /* D0 */);
//   if (!SD_MMC.begin(root, true /* mode1bit */, false /* format_if_mount_failed */, SDMMC_FREQ_HIGHSPEED))
// #elif defined(SD_CS)
// #define FILESYSTEM SD
//   Serial.println("mount SPI SD");
//   if (!SD.begin(SD_CS, SPI, 80000000, "/root"))
// #else
// #define FILESYSTEM FFat
//   Serial.println("mount FFat");
//   if (!FFat.begin(false, root))
//   // Serial.println("mount LittleFS");
//   // if (!LittleFS.begin(false, root))
//   // Serial.println("mount SPIFFS");
//   // if (!SPIFFS.begin(false, root))
// #endif
//   {
//     Serial.println("ERROR: File system mount failed!");
//   }
//   else
//   {
//     output_buf_size = gfx->width() * gfx->height() * 2;
// #if defined(RGB_PANEL) | defined(DSI_PANEL)
//     output_buf = gfx->getFramebuffer();
// #else
//     output_buf = (uint16_t *)aligned_alloc(16, output_buf_size);
// #endif
//     if (!output_buf)
//     {
//       Serial.println("output_buf aligned_alloc failed!");
//     }

//     avi_init();
//   }
// }

// void loop()
// {
//   Serial.printf("Open folder: %s\n", avi_folder);
//   File dir = FILESYSTEM.open(avi_folder);
//   if (!dir.isDirectory())
//   {
//     Serial.println("Not a directory");
//     delay(5000); // avoid error repeat too fast
//   }
//   else
//   {
//     File file = dir.openNextFile();
//     while (file)
//     {
//       if (!file.isDirectory())
//       {
//         std::string s = file.name();
//         // if ((!s.starts_with(".")) && (s.ends_with(".avi")))
//         if ((s.rfind(".", 0) != 0) && ((int)s.find(".avi", 0) > 0))
//         {
//           if (random(100) > 90)
//           {
//             s = root;
//             s += file.path();
//             if (avi_open((char *)s.c_str()))
//             {
//               Serial.println("AVI start");
//               gfx->fillScreen(RGB565_BLACK);

// #ifdef AVI_SUPPORT_AUDIO
// #ifdef AUDIO_MUTE
//               digitalWrite(AUDIO_MUTE, HIGH); // unmute
// #endif

//               if (avi_aRate > 0)
//               {
//                 i2s_set_sample_rate(avi_aRate);
//               }

//               avi_feed_audio();

//               if (avi_aFormat == PCM_CODEC_CODE)
//               {
//                 Serial.println("Start play PCM audio task");
//                 BaseType_t ret_val = pcm_player_task_start();
//                 if (ret_val != pdPASS)
//                 {
//                   Serial.printf("pcm_player_task_start failed: %d\n", ret_val);
//                 }
//               }
//               else if (avi_aFormat == MP3_CODEC_CODE)
//               {
//                 Serial.println("Start play MP3 audio task");
//                 BaseType_t ret_val = mp3_player_task_start();
//                 if (ret_val != pdPASS)
//                 {
//                   Serial.printf("mp3_player_task_start failed: %d\n", ret_val);
//                 }
//               }
//               else
//               {
//                 Serial.println("No audio task");
//               }
// #endif

//               avi_start_ms = millis();

//               Serial.println("Start play loop");
//               while (avi_curr_frame < avi_total_frames)
//               {
// #ifdef AVI_SUPPORT_AUDIO
//                 avi_feed_audio();
// #endif

//                 if (avi_decode())
//                 {
//                   avi_draw(0, 0);
//                 }
//               }

// #if defined(AVI_SUPPORT_AUDIO) && defined(AUDIO_MUTE)
//               digitalWrite(AUDIO_MUTE, LOW); // mute
// #endif

//               avi_close();
//               Serial.println("AVI end");

//               avi_show_stat();
//               delay(5000); // 5 seconds
//             }
//           }
//         }
//       }
//       file = dir.openNextFile();
//     }
//     dir.close();
//   }
// }
