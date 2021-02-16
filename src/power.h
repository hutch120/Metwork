#include <Arduino.h>

#ifndef _POWER_H
#define _POWER_H

/*
 * Handle the physical button press on the side of the T-Watch 2020
 */
void power_setup();
void power_loop();
bool isSleeping();

#endif // _POWER_H