#include <Arduino.h>

#ifndef _POWER_H
#define _POWER_H

/*
 * Handle the physical button press on the side of the T-Watch 2020
 */
void power_setup();
void button_loop();
bool isSleeping();

void wakeUp();
void sleep();

#endif // _POWER_H