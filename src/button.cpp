#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h"

AXP20X_Class *power;
bool bButtonPressed = false;
bool sleeping = false;

bool isSleeping()
{
    return sleeping;
}

void print_wakeup_reason()
{
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        log_i("Wakeup caused by external signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        log_i("Wakeup caused by external signal using RTC_CNTL");
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        log_i("Wakeup caused by timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        log_i("Wakeup caused by touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        log_i("Wakeup caused by ULP program");
        break;
    default:
        log_i("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
}

void button_setup()
{
    sleeping = false;
    print_wakeup_reason();

    TTGOClass *ttgo = TTGOClass::getWatch();
    power = ttgo->power;

    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(
        AXP202_INT, [] { bButtonPressed = true; }, FALLING);

    // Must be enabled first, and then clear the interrupt status,
    // otherwise abnormal
    power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);

    //  Clear interrupt status
    power->clearIRQ();
}

void button_loop()
{
    // Wait for the power button to be pressed
    if (bButtonPressed)
    {
        sleeping = true;
        log_i("Button pressed. Put device to sleep...");

        // After the AXP202 interrupt is triggered, the interrupt status must be cleared,
        // otherwise the next interrupt will not be triggered
        power->clearIRQ();

        TTGOClass *ttgo = TTGOClass::getWatch();
        for (int i = 5; i > 0; i--)
        {
            ttgo->tft->fillScreen(TFT_RED);
            ttgo->tft->setTextColor(TFT_WHITE);
            ttgo->tft->drawString(F("METWORK"), 1, 1);

            String countdown = "";
            countdown += "Restart in ";
            countdown += i;
            ttgo->tft->drawString(countdown, 10, 50, 2);
            delay(1000);
        }
        ttgo->tft->fillScreen(TFT_RED);
        ttgo->tft->drawString("Sleep Now...", 10, 50, 2);
        delay(1000);

        // Set screen and touch to sleep mode
        ttgo->displaySleep();
        ttgo->powerOff();

        esp_sleep_enable_gpio_wakeup();

        // esp_deep_sleep_start(); // See readme... requires other configuration.
        esp_light_sleep_start();
    }
}
