#ifndef _ESPNOW_H
#define _ESPNOW_H

/*
*  Start ESPNOW low level broadcasts / pings
*/
void espnow_setup();
void espnow_loop();
String getStats();

#endif // _ESPNOW_H