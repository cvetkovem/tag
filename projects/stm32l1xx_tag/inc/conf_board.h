#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "dev_hw_timer.h"
#include "dac.h"

/** Timer IRQ event handler **/
void TimerIrqHandler(void);

/** DAC settings **/


/** Board LED pin PC11 **/
#define BOARD_LED_BLUE_port           2 //2 - PORTC
#define BOARD_LED_BLUE_pin            11

/** Board BUTTON pin PC10 **/
#define BOARD_BUTTON_port             2 //2 - PORTC
#define BOARD_BUTTON_pin              10

/** Board POWEREN pin PD2 **/
#define BOARD_POWEREN_port             3 //3 - PORTD
#define BOARD_POWEREN_pin              2

#endif /* __CONF_BOARD_H_INCLUDED__ */
