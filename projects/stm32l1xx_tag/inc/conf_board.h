#ifndef __CONF_BOARD_H_INCLUDED__
#define __CONF_BOARD_H_INCLUDED__

#include <stdlib.h>
#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "gpio.h"
#include "dev_hw_timer.h"
#include "dac.h"
#include "dev_temp.h"
#include "i2c.h"
#include "dev_bat_measure.h"

/** Timer IRQ event handler **/
void TimerIrqHandler(void);

/** DAC settings **/


/** Led pin PC11 **/
#define BOARD_LED_BLUE_port           2 //2 - PORTC
#define BOARD_LED_BLUE_pin            11

/** Button pin PC10 **/
#define BOARD_BUTTON_port             2 //2 - PORTC
#define BOARD_BUTTON_pin              10

/** Power enable pin PD2 **/
#define BOARD_POWER_ENABLE_port       3 //3 - PORTD
#define BOARD_POWER_ENABLE_pin        2

/** Battery measurement pin PC2 **/
#define BOARD_BAT_MEAS_port           2 //2 - PORTC
#define BOARD_BAT_MEAS_pin            2

/** Battery ground pin PC1 **/
#define BOARD_BAT_GND_port            2 //2 - PORTC
#define BOARD_BAT_GND_pin             1

/** Charger connection status pin PB9 **/
#define BOARD_CHARGER_CONNECT_port    1 //1 - PORTB
#define BOARD_CHARGER_CONNECT_pin     9

/** VCO enable pin PA7 **/
#define BOARD_VCO_ENABLE_port         0 //0 - PORTA
#define BOARD_VCO_ENABLE_pin          7

/** PA enable pin PC5 **/
#define BOARD_PA_ENABLE_port          2 //2 - PORTC
#define BOARD_PA_ENABLE_pin           5

/** Tune pin PA5 **/
#define BOARD_TUNE_port               0 //0 - PORTA
#define BOARD_TUNE_pin                5

/** MAC I2C2 SCL pin PB10 **/
#define BOARD_MAC_SCL_port            1 //1 - PORTB
#define BOARD_MAC_SCL_pin             10

/** MAC I2C2 SDA pin PB11 **/
#define BOARD_MAC_SDA_port            1 //1 - PORTB
#define BOARD_MAC_SDA_pin             11

/** USART RX pin PA10 **/
#define BOARD_USART_RX_port           0 //0 - PORTA
#define BOARD_USART_RX_pin            10

/** USART TX pin PA9 **/
#define BOARD_USART_TX_port           0 //0 - PORTA
#define BOARD_USART_TX_pin            9


#endif /* __CONF_BOARD_H_INCLUDED__ */
