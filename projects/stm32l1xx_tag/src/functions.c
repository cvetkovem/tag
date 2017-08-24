#include "functions.h"

#define VREFINT_CAL           ((uint16_t *)(uint32_t)0x1ff800f8)
#define BATTERY_MIN_VOLTAGE   2500

Gpio_t LED_BLUE;
Gpio_t BUTTON;
Gpio_t POWER_ENABLE;

void buttonPressed(void);
void buttonReleased(void);

void deviceEnable() {
    cpuInit();

    // delay enable
    cpuDelay_ms(3000);

    if (getChargerConnectionStatus()) {
        return;
    }

    if (measureBatteryVoltage() < BATTERY_MIN_VOLTAGE) {
        return;
    }

    /* Power enable pin settings */
    POWER_ENABLE.pinIndex = BOARD_POWER_ENABLE_pin;
    POWER_ENABLE.portIndex = BOARD_POWER_ENABLE_port;
    GpioInit(&POWER_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1);

    /* LED pin settings */
    LED_BLUE.pinIndex = BOARD_LED_BLUE_pin;
    LED_BLUE.portIndex = BOARD_LED_BLUE_port;
    GpioInit(&LED_BLUE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* Button pin settings */
    BUTTON.pinIndex = BOARD_BUTTON_pin;
    BUTTON.portIndex = BOARD_BUTTON_port;
    GpioInit(&BUTTON, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GpioSetInterrupt(&BUTTON, IRQ_FALLING_EDGE, IRQ_HIGH_PRIORITY, buttonPressed);
    GpioSetInterrupt(&BUTTON, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, buttonReleased);

    if(!GpioRead(&BUTTON)) {
        GpioWrite(&LED_BLUE, 1);
    }

    readMacAddress(macAddress);

    /** Create a package template **/
    //TODO

    //start timer

    //sleep
}

void buttonPressed(void) {
    GpioWrite(&LED_BLUE, 1);
}

void buttonReleased(void) {
    GpioWrite(&LED_BLUE, 0);
}

void readMacAddress(uint8_t *macAddress) {
    I2C_t i2c;
    i2c.I2C = (uint32_t *)I2C2;
    i2c.Scl.pinIndex  = BOARD_MAC_SCL_pin;
    i2c.Scl.portIndex = BOARD_MAC_SCL_port;
    i2c.Sda.pinIndex  = BOARD_MAC_SDA_pin;
    i2c.Sda.portIndex = BOARD_MAC_SDA_port;

    I2C_init(&i2c);
    int i;
    for(i=0; i < 4; i++) __NOP();
    I2C_burst_read(&i2c, 0xA0, 0xFA, 6, macAddress);
    I2C_deInit(&i2c);
}

uint8_t getChargerConnectionStatus() {
    uint8_t status = 0;
    Gpio_t CHARGER_CONNECT;

    /* Charger connection status pin settings */
    CHARGER_CONNECT.pinIndex = BOARD_CHARGER_CONNECT_pin;
    CHARGER_CONNECT.portIndex = BOARD_CHARGER_CONNECT_port;
    GpioInit(&CHARGER_CONNECT, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    status = !GpioRead(&CHARGER_CONNECT);

    GpioDeInit(&CHARGER_CONNECT);

    return status;
}

uint16_t measureBatteryVoltage() {
    uint16_t battVoltage;

    Bat_measure_t bat_measure;
    bat_measure.bat_measure_pin.pinIndex = BOARD_BAT_MEAS_pin;
    bat_measure.bat_measure_pin.portIndex = BOARD_BAT_MEAS_port;
    bat_measure.bat_gnd_pin.pinIndex = BOARD_BAT_GND_pin;
    bat_measure.bat_gnd_pin.portIndex = BOARD_BAT_GND_port;

    batMeasure(&bat_measure);

    battVoltage = bat_measure.measurement;

    return battVoltage;
}

uint16_t measureTemp() {
  uint16_t tmp = 0;

  getTemp(&tmp);

  uint16_t vref = 0;
  vref = VREFINT_CAL;

  return tmp;
}

void TimerIrqHandler(void) {

}
