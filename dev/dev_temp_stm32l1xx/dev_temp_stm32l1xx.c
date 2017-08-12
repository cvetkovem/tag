#include "stm32l1xx.h"
#include "dev_cpu.h"
#include "dev_temp.h"

void getTemp(uint16_t *temp) {
  int i;
  uint16_t tmp;

  /* Enable HSI 16MHz */
  RCC->CR |= RCC_CR_HSION;
  while(!(RCC->CR&RCC_CR_HSIRDY));

  /* ADC clock enable */
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  /* ADC ON */
  ADC1->CR2 |= ADC_CR2_ADON;
  while(!(ADC1->SR&ADC_SR_ADONS));

  /* 16 channel */
  ADC1->SQR5 |= (ADC_SQR5_SQ1_4);

  /* 12-bit ADC */
  ADC1->CR1 &= ~ADC_CR1_RES;

  /* Align right */
  ADC1->CR2 &= ~ADC_CR2_ALIGN;

  ADC1->SMPR2 |= (ADC_SMPR2_SMP16_0 | ADC_SMPR2_SMP16_1 | ADC_SMPR2_SMP16_2 );
  ADC1->SMPR2 |= (ADC_SMPR2_SMP17_0 | ADC_SMPR2_SMP17_1 | ADC_SMPR2_SMP17_2 );

  /* On temp sensor */
  //ADC1->CCR |= ADC_CCR_TSVREFE;
  ((ADC_Common_TypeDef *)(ADC1_BASE + 0x300))->CCR |= ADC_CCR_TSVREFE;
  /* Wait until Vrefint stable */
  while(!(PWR->CSR & PWR_CSR_VREFINTRDYF));

  cpuDisable_irq();

  ADC1->CR2 |= ADC_CR2_SWSTART;
  while(!(ADC1->SR & ADC_SR_EOC));

  *temp = ADC1->DR;

  for(i = 0; i < 100; i++) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while(!(ADC1->SR & ADC_SR_EOC));

    tmp = ADC1->DR;
    *temp = (tmp + *temp) / 2;
  }

  cpuEnable_irq();


  /* Off temp sensor */
  //ADC1->CCR &= ~ADC_CCR_TSVREFE;
  ((ADC_Common_TypeDef *)(ADC1_BASE + 0x300))->CCR &= ~ADC_CCR_TSVREFE;

  /* ADC OFF */
  ADC1->CR2 &= ~ADC_CR2_ADON;

  /* ADC clock disable */
  RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN;

  /* Disable HSI */
  RCC->CR &= ~RCC_CR_HSION;
}
