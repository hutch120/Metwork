#include <Arduino.h>

#ifndef _SOUND_H
#define _SOUND_H

/*
 * Play a sound
 */
void sound_setup();
void sound_loop();
/**
 * @brief speak
 *  
 * @param   str    the text to be spoken
 */
void sound_speak(const char *str);
void sound_play_beep();

#endif // _SOUND_H