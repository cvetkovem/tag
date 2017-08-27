#ifndef __FUNCTIONS_H_INCLUDED__
#define __FUNCTIONS_H_INCLUDED__

#include "conf_board.h"

void deviceEnable();

void wakeUpAndTransmit();

void readMacAddress(uint8_t *macAddress);

uint8_t getChargerConnectionStatus();

uint16_t measureBatteryVoltage();

uint16_t measureTemp();

void powerOff();

uint8_t buttonPressedCheck();

#endif /* __FUNCTIONS_H_INCLUDED__ */
