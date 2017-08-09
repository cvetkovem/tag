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
#include "conf_board.h"

Gpio_t LED_BLUE;
Gpio_t BUTTON;
Gpio_t POWEREN;

void boardInit(void);
void buttonFunc(void);

int main(void) {
  boardInit();

  int i;
  while(1) {
    __NOP();
    //GpioWrite(&LED_BLUE, 1);
    //for(i=0; i < 32000000; i++) __NOP();
    //GpioWrite(&LED_BLUE, 0);
    //for(i=0; i < 32000000; i++) __NOP();
  }
}

void boardInit(void) {
    cpuInit();

    /* LED settings */
    LED_BLUE.pinIndex = BOARD_LED_BLUE_pin;
    LED_BLUE.portIndex = BOARD_LED_BLUE_port;
    GpioInit(&LED_BLUE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* PowerEN settings */
    POWEREN.pinIndex = BOARD_POWEREN_pin;
    POWEREN.portIndex = BOARD_POWEREN_port;
    GpioInit(&POWEREN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    /* Button settings */
    BUTTON.pinIndex = BOARD_BUTTON_pin;
    BUTTON.portIndex = BOARD_BUTTON_port;
    GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioSetInterrupt(&BUTTON, IRQ_FALLING_EDGE, IRQ_HIGH_PRIORITY, buttonFunc);

    //TimerHwInit();
}

volatile uint8_t led_status = false;
void buttonFunc(void) {
    if(!led_status) {
        GpioWrite(&LED_BLUE, 1);
    } else {
        GpioWrite(&LED_BLUE, 0);
    }

    led_status = !led_status;
}

void TimerIrqHandler(void) {

}
