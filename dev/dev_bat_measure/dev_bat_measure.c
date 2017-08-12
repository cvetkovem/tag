#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "dev_bat_measure.h"

/** measure PC2 **/

void batMeasure(Bat_measure_t *bat_measure) {
  int i;
  uint16_t tmp;

  /* Gpio settings */
  GpioInit(&bat_measure->bat_gnd_pin, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
  GpioInit(&bat_measure->bat_measure_pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);

  /* Enable HSI 16MHz */
  RCC->CR |= RCC_CR_HSION;
  while(!(RCC->CR&RCC_CR_HSIRDY));

  /* ADC clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  /* ADC ON */
  ADC1->CR2 |= ADC_CR2_ADON;
  while(!(ADC1->SR&ADC_SR_ADONS));

  /* 12 channel */
  ADC1->SQR5 |= (ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_3);

  /* 12-bit ADC */
  ADC1->CR1 &= ~ADC_CR1_RES;

  /* Align right */
  ADC1->CR2 &= ~ADC_CR2_ALIGN;

  ADC1->SMPR2 |= (ADC_SMPR2_SMP12_0 | ADC_SMPR2_SMP12_1 | ADC_SMPR2_SMP12_2 );

  cpuDisable_irq();

  ADC1->CR2 |= ADC_CR2_SWSTART;
  while(!(ADC1->SR & ADC_SR_EOC));

  bat_measure->measurement = ADC1->DR;

  for(i = 0; i < 10; i++) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC));

    tmp = ADC1->DR;
    bat_measure->measurement = (tmp + bat_measure->measurement) / 2;
  }

  cpuEnable_irq();

  /* ADC OFF */
  ADC1->CR2 &= ~ADC_CR2_ADON;

  /* ADC clock disable */
  RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;

  /* Disable HSI */
  RCC->CR &= ~RCC_CR_HSION;

  GpioDeInit(&bat_measure->bat_measure_pin);
  GpioDeInit(&bat_measure->bat_gnd_pin);
}
