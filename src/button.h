#include <Arduino.h>

#ifndef _BUTTON_H
#define _BUTTON_H

/*
 * Handle the physical button press on the side of the T-Watch 2020
 */
void button_setup();
void button_loop();
bool isSleeping();

#endif // _BUTTON_H