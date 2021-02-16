#include "config.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TTGO.h>
#include "esp_wifi_types.h"
#include "espnow.h"

#define Threshold 40 /* Greater the value, more the sensitivity */

AXP20X_Class *power;
bool buttonPressed = false;
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
        log_i("Wakeup was not caused by deep sleep: %d", wakeup_reason);
        break;
    }
}

// Execute this function when Touch Pad in pressed
void touchCallback()
{
    // wake up on next loop
    if (sleeping)
    {
        log_i("touchCallback wake up on next loop");
        buttonPressed = true;
    }
    else
    {
        log_i("touchCallback");
    }
}

void power_setup()
{
    sleeping = false;
    print_wakeup_reason();

    TTGOClass *ttgo = TTGOClass::getWatch();
    power = ttgo->power;

    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(
        AXP202_INT, [] { buttonPressed = true; }, FALLING);

    touchAttachInterrupt(TOUCH_INT, touchCallback, Threshold);

    // Must be enabled first, and then clear the interrupt status,
    // otherwise abnormal
    power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);

    //  Clear interrupt status
    power->clearIRQ();

    esp_err_t gpio_wakeup_enable_res = gpio_wakeup_enable((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
    log_i("gpio_wakeup_enable_res %d", gpio_wakeup_enable_res);
    esp_err_t esp_sleep_enable_gpio_wakeup_res = esp_sleep_enable_gpio_wakeup();
    log_i("esp_sleep_enable_gpio_wakeup_res %d", esp_sleep_enable_gpio_wakeup_res);
    esp_err_t esp_sleep_enable_touchpad_wakeup_res = esp_sleep_enable_touchpad_wakeup(); // Doesnt seem to work.
    log_i("esp_sleep_enable_touchpad_wakeup_res %d", esp_sleep_enable_touchpad_wakeup_res);
}

void buttonSleep()
{
    log_i("Button pressed. Was awake.. put device to sleep ...");

    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->tft->fillScreen(TFT_RED);
    ttgo->tft->drawString("Sleep Now...", 10, 50, 2);
    delay(500);

    esp_err_t esp_wifi_stop_res = esp_wifi_stop();
    log_i("esp_wifi_stop_res %d", esp_wifi_stop_res);

    // Set screen and touch to sleep mode
    ttgo->bl->adjust(0);
    ttgo->displaySleep();
    ttgo->closeBL();
    ttgo->powerOff();

    Serial.flush();
    // esp_deep_sleep_start(); // See readme... requires other configuration.
    ttgo->powerOff();
    // esp_light_sleep_start();
}

void buttonWake()
{
    log_i("Button pressed. Was asleep.. wake up device ...");

    TTGOClass *ttgo = TTGOClass::getWatch();

    esp_err_t esp_wifi_start_res = esp_wifi_start();
    log_i("esp_wifi_start_res %d", esp_wifi_start_res);

    // Set screen and touch to normal mode
    ttgo->openBL();
    ttgo->displayWakeup();
    ttgo->bl->adjust(100);

    ttgo->tft->fillScreen(TFT_RED);
    ttgo->tft->drawString("Waking up...", 10, 50, 2);
    espnow_setup();
}

void power_loop()
{
    // Wait for the power button to be pressed
    if (buttonPressed)
    {
        buttonPressed = false;
        // After the AXP202 interrupt is triggered, the interrupt status must be cleared,
        // otherwise the next interrupt will not be triggered
        power->clearIRQ();

        sleeping = !sleeping;
        if (sleeping)
            buttonSleep();
        if (!sleeping)
            buttonWake();
    }
}
