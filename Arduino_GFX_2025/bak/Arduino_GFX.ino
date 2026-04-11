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
String currentGIF = GIF_FILENAME; // default as GIF_FILENAME
bool prevButton0 = HIGH;
bool prevButton47 = HIGH;
gd_GIF *gif_player = NULL;
File gifFile;
uint8_t *buf;

bool canProceed = true;

int32_t start_ms = millis(), t_delay = 0, delay_until;
int32_t res = 1;
int32_t duration = 0;

void loadGIF(const String &filename) {
  // will only play when spiffs is started up properly.
  gifFile = SPIFFS.open(filename, "r");

  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // check if the gif player object is not null and playing
    if (gif_player) {
      gifClass.gd_close_gif(gif_player); // close the gif player
      gif_player = NULL; // reset to null
    }

    // check complete, start to read the file

    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      uint8_t *buf = (uint8_t *)malloc(gif->width * gif->height);
      if (!buf)
      {
        Serial.println(F("buf malloc failed!"));
      }
      else
      {
        int16_t x = (gfx->width() - gif->width) / 2;
        int16_t y = (gfx->height() - gif->height) / 2;

        Serial.println(F("GIF video start"));
        int32_t start_ms = millis(), t_delay = 0, delay_until;
        int32_t res = 1;
        int32_t duration = 0, remain = 0;

        while (res > 0)
        {
          t_delay = gif->gce.delay * 10;
          res = gifClass.gd_get_frame(gif, buf);
          if (res < 0)
          {
            Serial.println(F("ERROR: gd_get_frame() failed!"));
            break;
          }
          else if (res > 0)
          {
            gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);

            duration += t_delay;
            delay_until = start_ms + duration;
            while (millis() < delay_until)
            {
              delay(1);
              remain++;
            }
          }
        }
        Serial.println(F("GIF video end"));
        Serial.print(F("Actual duration: "));
        Serial.print(millis() - start_ms);
        Serial.print(F(", expected duration: "));
        Serial.print(duration);
        Serial.print(F(", remain: "));
        Serial.print(remain);
        Serial.print(F(" ("));
        Serial.print(100.0 * remain / duration);
        Serial.println(F("%)"));

        gifClass.gd_close_gif(gif);
        free(buf);

      }

    }
  }
}


void loadGIF2(const String &filename) {

  // open the gif file
  canProceed = false;
  gifFile = SPIFFS.open(filename, "r");

  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }

  Serial.println("Decoding the GIF: " + filename);
  gif_player = gifClass.gd_open_gif(&gifFile);
  if (!gif_player) {
    Serial.println("Failed to decode GIF: " + filename);
    gifFile.close();
    return;
  }

  Serial.println("Successfully opened GIF: " + filename);
  canProceed = true;
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

  loadGIF2(currentGIF);
  //Serial.println("Hello world");
  //Serial.println("Starting GIF: " + currentGIF);
  //loadGIF(currentGIF);
  //isPlaying = true;
}

void stopGIF(uint8_t *buf) {

  if (gif_player) {
    Serial.println("KIlling GIF player");
    gifClass.gd_close_gif(gif_player);
    Serial.println("Freeing the buffer");
    free(buf);
    gfx->fillScreen(RGB565_BLACK);
    //gif_player = NULL;
    gifFile.close();
    isPlaying = false;
  }

}


void loop() {
  // Try to open the filename
  //Serial.println("loop");
  if (!canProceed) {
    Serial.println("No SPIFFS, cannot play");
    gfx->println(F("No SPIFFS, cannot play"));

  } 
  else {

    // Play GIF if active
    if (isPlaying && gif_player) {
      buf = (uint8_t *)malloc(gif_player->width * gif_player->height);
      

      if (!buf){
        // cannot assign buf, try to replay
        Serial.println("Unable to get buffer, seems video ended");

        gifClass.gd_rewind(gif_player); // replay the video
//        stopGIF(buf);
//        
//        isPlaying = false;
//
//        
//        loadGIF2(currentGIF);
//        isPlaying = true;
      }
      else
      {
        // yet to be finished.
        int16_t x = (gfx->width() - gif_player->width) / 2;
        int16_t y = (gfx->height() - gif_player->height) / 2;

        Serial.println(F("GIF video plays"));
      
        t_delay = gif_player->gce.delay * 10;
        res = gifClass.gd_get_frame(gif_player, buf);
        if (res < 0)
        {
           Serial.println(F("ERROR: gd_get_frame() failed!"));

           // stop playing if gd_get_frame failed
           stopGIF(buf);
//           
           isPlaying = false;
//           free(buf);
//           
//           loadGIF2(currentGIF);
//           isPlaying = true;
        }
        else if (res > 0)
        {
           //gfx->drawIndexedBitmap(x, y, buf, gif_player->palette->colors, gif_player->width, gif_player->height);

           if (millis() < delay_until){
              delay(1);
           }
           else
           {
              // time is up, update the delay interval and draw the new frame out.
              t_delay = gif_player->gce.delay * 10;
              delay_until = start_ms + duration;

              gfx->drawIndexedBitmap(x, y, buf, gif_player ->palette->colors, gif_player->width, gif_player->height);
           
           }
        }


//      while (res > 0)
//        {
//          t_delay = gif->gce.delay * 10;
//          res = gifClass.gd_get_frame(gif, buf);
//          if (res < 0)
//          {
//            Serial.println(F("ERROR: gd_get_frame() failed!"));
//            break;
//          }
//          else if (res > 0)
//          {
//            gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);
//
//            duration += t_delay;
//            delay_until = start_ms + duration;
//            while (millis() < delay_until)
//            {
//              delay(1);
//              remain++;
//            }
//          }
//        }


    // check state of button 0 and do necessary behavior
    bool btn0State = digitalRead(BUTTON0_PIN);
    if (btn0State == LOW && prevButton0 == HIGH) {
      isPlaying = false;
      Serial.println("Stopping GIF");
      if (gif_player)
        stopGIF(buf);
      //      if (gif_player) {
      //        Serial.println("KIlling GIF player");
      //        gifClass.gd_close_gif(gif_player);
      //        gfx->fillScreen(RGB565_BLACK);
      //        gif_player = NULL;
      //        isPlaying = false;
      //      }

    }

    prevButton0 = btn0State;

    // check button 47 and do necessary behavior

    bool btn47State = digitalRead(BUTTON47_PIN);
    if (btn47State == LOW && prevButton47 == HIGH) {
      if (gif_player)
        stopGIF(buf);
      isPlaying = false; // Stop current playback
      //gfx->fillScreen(RGB565_BLACK);

      // switch between GIF
      currentGIF = (currentGIF == GIF_FILENAME) ? GIF_FILENAME2 : GIF_FILENAME;
      Serial.println("Switching to GIF: " + currentGIF);

      // load the GIF
      loadGIF2(currentGIF);
      isPlaying = true;


    }

    prevButton47 = btn47State;

    
    }
      



      
    }


  }

}
