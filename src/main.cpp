#include "config.h"

#include <Arduino.h>
#include <TTGO.h>
#include "espnow.h"
#include "button.h"

// C++ object which will allow access to the functions of the Watch

lv_style_t style;

void setup()
{
  Serial.begin(115200);
  log_i("Setup"); // https://thingpulse.com/esp32-logging/
  delay(1000);

  TTGOClass *ttgo = TTGOClass::getWatch();

  // Get Watch object and set up the display
  ttgo->begin();

  ttgo->openBL();
  // Use SPI_eTFT library to display text on screen
  ttgo->tft->fillScreen(TFT_RED);
  ttgo->tft->setTextFont(2);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);

  //Check if the RTC clock matches, if not, use compile time
  ttgo->rtc->check();

  //Synchronize time to system time
  ttgo->rtc->syncToSystem();

  espnow_setup();
  button_setup();
}

void loop()
{
  if (!isSleeping())
  {
    log_i("Main loop running ...");
    // button_loop();

    espnow_loop();

    delay(5000);
  }
  delay(1000);
}