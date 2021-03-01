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

void initializeUI() 
{
     TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->tft->setTextFont(2);
    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_GREEN);
    ttgo->tft->fillScreen(TFT_BLACK);
}

void initializeTouchUI()
{
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->tft->fillCircle(120, 120, 65, TFT_LIGHTGREY);
}

void removeTouchUI()
{
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->tft->fillCircle(120, 120, 95, TFT_BLACK);
}

void drawTouchUI()
{
    if (refreshing) return;
    refreshing = true;

    TTGOClass *ttgo = TTGOClass::getWatch();
    
    //ttgo->tft->fillScreen(TFT_GREEN);
    int r = getTickTouch() % 5;
    // ttgo->tft->fillCircle(120, 120, 90, TFT_BLACK);
    for (int i = 1; i < 30; i++) {
        ttgo->tft->drawCircle(120, 120, 65 + i, TFT_BLACK);
    }

    ttgo->tft->drawCircle(120, 120, 65 + r * 6, TFT_DARKGREY);
    // ttgo->tft->fillCircle(120, 120, 65, TFT_GREEN);

    refreshing = false;
}

void drawWakeUpUI()
{
    if (refreshing) return;
    refreshing = true;

    TTGOClass *ttgo = TTGOClass::getWatch();
    
    if (isTouched()) {
        // touched down
        ttgo->tft->fillCircle(120, 120, 65, TFT_DARKGREY);
    }
    refreshing = false;
}

void drawStatsUI()
{
    if (refreshing) return;
    refreshing = true;   

    TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->tft->fillRect(1, 1, 240, 20, TFT_BLACK);
    ttgo->tft->setTextColor(TFT_DARKGREY);
    
    String stats = getStats();
    ttgo->tft->drawString(stats, 5, 5);

    int percentage = ttgo->power->getBattPercentage();
    if (percentage > 99) percentage = 99;
    String batteryStat = "";
    batteryStat += percentage;
    batteryStat += "%";
    ttgo->tft->drawString(batteryStat, 208, 5, 2);

    refreshing = false;
}