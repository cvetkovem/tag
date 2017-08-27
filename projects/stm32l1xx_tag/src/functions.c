#include "functions.h"

#define VREFINT_CAL             ((uint16_t *)(uint32_t)0x1ff800f8)
#define BATTERY_MIN_VOLTAGE     1000
#define SLEEP_TIME_MS           5000
#define SLEEP_TIME_USE_RAND     1

void buttonPressedReleased(void);
uint32_t getSleepTimeMs();
uint16_t shiftRegFunc();

/** D8:80:39:E4:B3:66 **/
uint8_t macAddress[6] = { 0 };
uint8_t packageNumber = 0;
uint16_t sReg = 0;

Gpio_t LED_BLUE;
Gpio_t BUTTON;
Gpio_t POWER_ENABLE;

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
    GpioSetInterrupt(&BUTTON, IRQ_RISING_FALLING_EDGE, IRQ_HIGH_PRIORITY, buttonPressedReleased);

    cpuEnable_irq();

    if(!GpioRead(&BUTTON)) {
        GpioWrite(&LED_BLUE, 1);
    }

    readMacAddress(macAddress);
    sReg = ((macAddress[4] << 8) & (0xFF00)) | macAddress[5];

    /** Create a package template **/
    //TODO

    //set alarm
    //getSleepTimeMs();

}

void buttonPressedReleased(void) {
    if(!GpioRead(&BUTTON)) {
        cpuInit();
        GpioWrite(&LED_BLUE, 1);
        //start timer
        TimerHwInit();
        TimerHwStart(3500000);
    } else {
        GpioWrite(&LED_BLUE, 0);
        //stop timer
        TimerHwStop();
        TimerHwDeInit();
    }
}

void wakeUpAndTransmit() {
    Gpio_t VCO_ENABLE;
    Gpio_t PA_ENABLE;

    cpuInit();

    if (getChargerConnectionStatus()) {
        powerOff();
    }

    if (measureBatteryVoltage() < BATTERY_MIN_VOLTAGE) {
        powerOff();
    }

    //create package


    /* Vco enable pin settings */
    VCO_ENABLE.pinIndex = BOARD_VCO_ENABLE_pin;
    VCO_ENABLE.portIndex = BOARD_VCO_ENABLE_port;
    GpioInit(&VCO_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    /* PA enable pin settings */
    PA_ENABLE.pinIndex = BOARD_PA_ENABLE_pin;
    PA_ENABLE.portIndex = BOARD_PA_ENABLE_port;
    GpioInit(&PA_ENABLE, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);

    //transmit start


    //transmit end
    GpioWrite(&PA_ENABLE, 0);
    GpioWrite(&VCO_ENABLE, 0);
    GpioDeInit(&PA_ENABLE);
    GpioDeInit(&VCO_ENABLE);

    //set alarm

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

void powerOff() {
    GpioWrite(&POWER_ENABLE, 0);
}

uint32_t getSleepTimeMs() {
#ifdef SLEEP_TIME_USE_RAND
    uint16_t rand16bit;
    float rand16bitToFloat;
    float timeSleepMs;

    rand16bit = shiftRegFunc();
    rand16bitToFloat = (float)rand16bit;

    timeSleepMs = (rand16bitToFloat / 65535.0f) * (float)SLEEP_TIME_MS;

    return (uint32_t)timeSleepMs;
#else
    return (uint32_t)SLEEP_TIME_MS;
#endif
}

uint16_t shiftRegFunc() {
  sReg = ((((sReg >> 15) ^ (sReg >> 13) ^ (sReg >> 12) ^ (sReg >> 10) ^ sReg ) & 0x0001 ) << 15 ) | (sReg >> 1);

  return sReg;
}

uint8_t buttonPressedCheck() {
    return !GpioRead(&BUTTON);
}

void TimerIrqHandler(void) {
    if(!GpioRead(&BUTTON)) {
        GpioWrite(&LED_BLUE, 0);
        GpioWrite(&POWER_ENABLE, 0);
    } else {
        TimerHwDeInit();
    }
}