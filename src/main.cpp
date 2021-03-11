#include "config.h"

#include <Arduino.h>
#include <TTGO.h>
#include "espnow.h"
#include "power.h"
#include "sound.h"
#include "touch.h"
#include "ui.h"

// C++ object which will allow access to the functions of the Watch

lv_style_t style;

int tick = 0;
int tickSleepTimeout;

void setup()
{
  Serial.begin(115200);
  log_i("Setup"); // https://thingpulse.com/esp32-logging/
  delay(1000);

  TTGOClass *ttgo = TTGOClass::getWatch();

  // Get Watch object and set up the display
  ttgo->begin();

  ttgo->openBL();

  //Check if the RTC clock matches, if not, use compile time
  ttgo->rtc->check();

  //Synchronize time to system time
  ttgo->rtc->syncToSystem();

  initializeUI();
  power_setup();
  espnow_setup();
  sound_setup();
  initializeTouch();

  tick = 0;
}

void loop()
{
  tick++;

  // main loop
  if (!isSleeping())
  {
    // awake, game time!
    // log_i("Awake ... running touch loop");

    // log_i("Touch loop...");
    touch_loop();
    sound_loop();

    // check for sleep timeout
    if (isTouched())
    {

      // touched
      if (getTickTouch() == 1)
      {
        drawStatsUI();
        initializeTouchUI();
      }

      drawTouchUI();

      if (tick % 5 == 0 || getTickTouch() == 1)
      {
        // every 1s
        // log_i("Comms loop...");
        espnow_loop();
      }
      tickSleepTimeout = 0;
    }
    else
    {

      // not touched
      tickSleepTimeout++;
      if (tickSleepTimeout == 1)
      {
        removeTouchUI();
        drawStatsUI();
      }

      /// log_i("No touch ... tickSleepTimeout %d", tickSleepTimeout);

      if (tickSleepTimeout > 500)
      {
        // timeout after 5s
        tickSleepTimeout = 0;
        sleep();
      }

      if (tick % 10 == 0)
      {
        // every 1s
        // log_i("Power loop...");
        button_loop();
      }
    }

    tick = tick % 300;
    delay(100);
  }
  else
  {
    // sleeping, only detect button press
    // log_i("Sleeping ...");

    // log_i("Touch loop...");
    touch_loop();

    if (isTouched())
    {
      wakeUp();
      espnow_setup();
      sound_setup();
      drawWakeUpUI();
    }

    button_loop();
    delay(1000);
  }
}
