#include <Arduino.h>

#ifndef _TOUCH_H
#define _TOUCH_H

void initializeTouch();
void touch_loop();
bool isTouched();
int getTickTouch();

#endif // _TOUCH_H