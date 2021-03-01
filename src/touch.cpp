#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h"
#include "espnow.h"

bool touched = false;
int tickTouch = 0;

bool isTouched()
{
    return touched;
}

int getTickTouch()
{
    return tickTouch;
}

void onTouchStart()
{
    log_i("Screen touch start.");
    touched = true;
    tickTouch = 0;
}

void onTouchEnd()
{
    log_i("Screen touch end.");
    touched = false;
    tickTouch = 0;
}

void initializeTouch()
{
    log_i("Setting up touch interactivity.");

    //TTGOClass *ttgo = TTGOClass::getWatch();
    //ttgo->initTouch();
    //touchAttachInterrupt(TOUCH_INT, onTouch, 50);
}

void touch_loop() 
{
    TTGOClass *ttgo = TTGOClass::getWatch();
    int16_t x, y;
    if (ttgo->getTouch(x, y)) {
        // log_i("Screen touched.");
        if (touched != true) {
            onTouchStart();
        }
        tickTouch++;
        // log_i("Touch count %d", tickTouch);
    }
    else {
        if (touched == true) {
            onTouchEnd();
        }
    }
}