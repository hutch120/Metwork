#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h"
#include "espnow.h"
#include "touch.h"

bool refreshing = false;

void drawTouchUI()
{
    if (refreshing) return;
    refreshing = true;

    TTGOClass *ttgo = TTGOClass::getWatch();
    
    if (isTouched()) {
        // touched down
        ttgo->tft->fillScreen(TFT_GREEN);
    }
    else {
        ttgo->tft->fillScreen(TFT_BLACK);
    }

    refreshing = false;
}

void drawWakeUpUI()
{
    if (refreshing) return;
    refreshing = true;

    TTGOClass *ttgo = TTGOClass::getWatch();
    
    if (isTouched()) {
        // touched down
        ttgo->tft->fillScreen(TFT_DARKGREEN);
    }
    refreshing = false;
}

void drawStatsUI()
{
    if (refreshing) return;
    refreshing = true;   

    TTGOClass *ttgo = TTGOClass::getWatch();

     if (isTouched()) {
        // touched down
        ttgo->tft->fillScreen(TFT_GREEN);
        ttgo->tft->setTextColor(TFT_BLACK);
    }
    else {
        ttgo->tft->fillScreen(TFT_BLACK);
         ttgo->tft->setTextColor(TFT_GREEN);
    }
    
    String stats = getStats();
    ttgo->tft->drawString(stats, 5, 5);

    int percentage = ttgo->power->getBattPercentage();
    String batteryStat = "";
    batteryStat += percentage;
    batteryStat += "%";
    ttgo->tft->drawString(batteryStat, 180, 5, 2);

    refreshing = false;
}