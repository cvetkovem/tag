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

#define VREFINT_CAL           ((uint16_t *)(uint32_t)0x1ff800f8)

Gpio_t LED_BLUE;
Gpio_t BUTTON;
Gpio_t POWER_ENABLE;
Gpio_t VCO_ENABLE;
Gpio_t PA_ENABLE;
Gpio_t CHARGER_CONNECT;

void boardInit(void);
void buttonFunc(void);

int main(void) {

  cpuDelay_ms(100);

  boardInit();

  uint16_t tmp = 0;

  getTemp(&tmp);

  uint16_t vref = 0;
  vref = VREFINT_CAL;

  while(1) {
    __NOP();

    // test charger enable status
    if(!GpioRead(&CHARGER_CONNECT)) {
      GpioWrite(&POWER_ENABLE, 0);
    }
    //GpioWrite(&LED_BLUE, 1);
    //for(i=0; i < 32000000; i++) __NOP();
    //GpioWrite(&LED_BLUE, 0);
    //for(i=0; i < 32000000; i++) __NOP();
  }
}

void boardInit(void) {
    cpuInit();

    /* LED pin settings */
    LED_BLUE.pinIndex = BOARD_LED_BLUE_pin;
    LED_BLUE.portIndex = BOARD_LED_BLUE_port;
    GpioInit(&LED_BLUE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Power enable pin settings */
    POWER_ENABLE.pinIndex = BOARD_POWER_ENABLE_pin;
    POWER_ENABLE.portIndex = BOARD_POWER_ENABLE_port;
    GpioInit(&POWER_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    /* Vco enable pin settings */
    VCO_ENABLE.pinIndex = BOARD_VCO_ENABLE_pin;
    VCO_ENABLE.portIndex = BOARD_VCO_ENABLE_port;
    GpioInit(&VCO_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* PA enable pin settings */
    PA_ENABLE.pinIndex = BOARD_PA_ENABLE_pin;
    PA_ENABLE.portIndex = BOARD_PA_ENABLE_port;
    GpioInit(&PA_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Button pin settings */
    BUTTON.pinIndex = BOARD_BUTTON_pin;
    BUTTON.portIndex = BOARD_BUTTON_port;
    GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioSetInterrupt(&BUTTON, IRQ_FALLING_EDGE, IRQ_HIGH_PRIORITY, buttonFunc);

    /* Charger connection status pin settings */
    CHARGER_CONNECT.pinIndex = BOARD_CHARGER_CONNECT_pin;
    CHARGER_CONNECT.portIndex = BOARD_CHARGER_CONNECT_port;
    GpioInit(&CHARGER_CONNECT, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /** READ MAC **/
    I2C_t i2c;
    i2c.I2C = (uint32_t *)I2C2;
    i2c.Scl.pinIndex  = BOARD_MAC_SCL_pin;
    i2c.Scl.portIndex = BOARD_MAC_SCL_port;
    i2c.Sda.pinIndex  = BOARD_MAC_SDA_pin;
    i2c.Sda.portIndex = BOARD_MAC_SDA_port;

    /** D8:80:39:E4:B3:66 **/
    uint8_t MAC_Address[6] = { 0 };
    I2C_init(&i2c);
    int i;
    for(i=0; i < 4; i++) __NOP();
    I2C_burst_read(&i2c, 0xA0, 0xFA, 6, MAC_Address);

    I2C_deInit(&i2c);

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
