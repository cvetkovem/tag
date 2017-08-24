#ifndef __FUNCTIONS_H_INCLUDED__
#define __FUNCTIONS_H_INCLUDED__

#include "conf_board.h"

/** D8:80:39:E4:B3:66 **/
uint8_t macAddress[6] = { 0 };

uint8_t packageNumber = 0;


void deviceEnable();

void readMacAddress(uint8_t *macAddress);

uint8_t getChargerConnectionStatus();

uint16_t measureBatteryVoltage();

uint16_t measureTemp();

#endif /* __FUNCTIONS_H_INCLUDED__ */
