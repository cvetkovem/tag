#ifndef __DEV_BAT_MEASURE_H_INCLUDED__
#define __DEV_BAT_MEASURE_H_INCLUDED__

#include "gpio.h"

typedef struct Bat_measure_t
{
  uint16_t measurement;
  Gpio_t bat_measure_pin;
  Gpio_t bat_gnd_pin;
} Bat_measure_t;

void batMeasure(Bat_measure_t *bat_measure);

#endif /* __DEV_BAT_MEASURE_H_INCLUDED__ */
