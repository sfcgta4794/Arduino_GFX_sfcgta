#include <Arduino_DataBus.h>
#include <Arduino_G.h>
#include <Arduino_GFX.h>
#include <Arduino_GFX_Library.h>
#include <Arduino_TFT.h>
#include <Arduino_TFT_18bit.h>
#include <YCbCr2RGB.h>
#include <gfxfont.h>

#include <FFat.h>
#include <LittleFS.h>
#include <SPIFFS.h>
//#include <SD.h>
//#include <SD_MMC.h>

#include "GifClass.h"
static GifClass gifClass;

/* OPTION 1: Uncomment a dev device in Arduino_GFX_dev_device.h */
#include "Arduino_GFX_dev_device.h"

#ifndef GFX_DEV_DEVICE
/* OPTION 2: Manual define hardware */

/* Step 1: Define pins in Arduino_GFX_databus.h */
#include "Arduino_GFX_pins.h"

/* Step 2: Uncomment your databus in Arduino_GFX_databus.h */
#include "Arduino_GFX_databus.h"

/* Step 3: Uncomment your display driver in Arduino_GFX_display.h */
#include "Arduino_GFX_display.h"

#endif

#define GIF_FILENAME "/JehovahShalom_short2.gif"
#define GIF_FILENAME2 "/JehovahZNSE_short2.gif"
// Button pins
#define BUTTON0_PIN 0   // Pause/Play button
#define BUTTON47_PIN 47 // Switch GIF button

// GIF player variables
bool isPlaying = true;
bool shouldStop = false;
bool shouldSwitch = false;

String currentGIF = GIF_FILENAME; // default as GIF_FILENAME
bool btn0State = LOW, btn47State = LOW;
bool prevButton0 = HIGH;
bool prevButton47 = HIGH;
gd_GIF *gif = NULL;
File gifFile;
uint8_t *buf;
int currentGif_id = 1; // 1 for GIF1, 2 for GIF2

bool canProceed = true;

int32_t start_ms = millis(), t_delay = 0, delay_until;
unsigned long nextFrameTime = 0;
int32_t res = 1;
int32_t duration = 0;
int16_t x = 0, y = 0;

void loadGIF(const String &filename){
  // stop playback
  stopGIF();

  gifFile = SPIFFS.open(filename, "r");

  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
    return;
  }

  // Open GIF
  gif = gifClass.gd_open_gif(&gifFile);
  if (!gif) {
    Serial.println("ERROR: Failed to open GIF");
    gifFile.close();
    return;
  }

  // Allocate buffer
  buf = (uint8_t*)malloc(gif->width * gif->height);
  if (!buf) {
    Serial.println("ERROR: Failed to allocate buffer");
    gifClass.gd_close_gif(gif);
    gifFile.close();
    return;
  }

  // Calculate centering position
  x = (gfx->width() - gif->width) / 2;
  y = (gfx->height() - gif->height) / 2;

  // Get first frame
  res = gifClass.gd_get_frame(gif, buf);
  if (res > 0) {
    gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);
    nextFrameTime = millis() + (gif->gce.delay * 10);
    isPlaying = true;
  } else {
    Serial.println("ERROR: No frames in GIF");
    stopGIF();
  }




}

void setup() {
  // put your setup code here, to run once:
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);

  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }

  gfx->fillScreen(RGB565_BLACK);

  // #ifdef GFX_BL
  //   pinMode(GFX_BL, OUTPUT);
  //   digitalWrite(GFX_BL, HIGH);
  // #endif

  if (!SPIFFS.begin()) {
    Serial.println("Unable to init SPIFFS!");
    canProceed = false;
  }
  else {
    Serial.println("Init SPIFFS completed.");

  }

  // Initialize buttons
  pinMode(BUTTON0_PIN, INPUT_PULLUP); // button 0: stop button
  pinMode(BUTTON47_PIN, INPUT_PULLUP); // button 47: switch to other

  loadGIF(currentGIF);
}

void stopGIF(){
  Serial.println("Killing GIF player with stopGIF");
  if (gif) {
    Serial.println("Closing the GIF");
    gifClass.gd_close_gif(gif);
  }

  
  if (buf){
    Serial.println("Closing the buffer");
    free(buf);
  }

  if (gifFile)
  {
    Serial.println("Closing the GIF file");
    gifFile.close();
  }

  isPlaying = false;
  shouldStop = false;
  shouldSwitch = false;

  gfx->fillScreen(RGB565_BLACK);
}

bool isButtonPressed(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(50); // Simple debounce
    if (digitalRead(pin) == LOW) {
      while (digitalRead(pin) == LOW); // Wait for release
      return true;
    }
  }
  return false;
}

void loop(){
  
  // stop button
  btn0State = isButtonPressed(BUTTON0_PIN);
  if (btn0State && !prevButton0) {
    shouldStop = true;
  }

  prevButton0 = btn0State;

  // switch GIF button
  btn47State = isButtonPressed(BUTTON47_PIN);
  if (btn47State && !prevButton47) {
    shouldSwitch = true;
    currentGif_id = (currentGif_id == 1) ? 2 : 1;
  }

  prevButton47 = btn47State;

  // Handle stop or switch
  if ((shouldStop || shouldSwitch) && isPlaying) {
    //stopGIF();
    if (shouldSwitch) {
      if (currentGif_id == 1)
        loadGIF(GIF_FILENAME);
      else
        loadGIF(GIF_FILENAME2);
    } else if (shouldStop){
      stopGIF();
    }
  }

  // Play next frame if time
  if (isPlaying && millis() >= nextFrameTime) {
    res = gifClass.gd_get_frame(gif, buf);
    if (res > 0) {
      gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);
      nextFrameTime = millis() + (gif->gce.delay * 10);
    } else {
      //stopGIF();
      loadGIF(currentGif_id == 1 ? GIF_FILENAME : GIF_FILENAME2); // Loop same GIF
    }
  }

  // Prevent excessive CPU usage
  delay(1);
}