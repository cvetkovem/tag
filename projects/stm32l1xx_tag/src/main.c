/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "functions.h"

Gpio_t VCO_ENABLE;
Gpio_t PA_ENABLE;

int main(void) {
    deviceEnable();

    while(1) {
        __NOP();
    }
}

void boardInit(void) {
    cpuInit();

    Gpio_t VCO_ENABLE;
    Gpio_t PA_ENABLE;

    /* Vco enable pin settings */
    VCO_ENABLE.pinIndex = BOARD_VCO_ENABLE_pin;
    VCO_ENABLE.portIndex = BOARD_VCO_ENABLE_port;
    GpioInit(&VCO_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* PA enable pin settings */
    PA_ENABLE.pinIndex = BOARD_PA_ENABLE_pin;
    PA_ENABLE.portIndex = BOARD_PA_ENABLE_port;
    GpioInit(&PA_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);


    //TimerHwInit();
}
