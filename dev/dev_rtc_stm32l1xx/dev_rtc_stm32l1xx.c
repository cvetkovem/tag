#include "stm32l1xx.h"
#include "rtc.h"

#define RTC_INIT_MASK           ((uint32_t)0xFFFFFFFF)
#define RTC_RSF_MASK            ((uint32_t)0xFFFFFF5F)

#define INITMODE_TIMEOUT         ((uint32_t) 0x00002000)
#define SYNCHRO_TIMEOUT          ((uint32_t) 0x00008000)

#define RCC_OFFSET               (RCC_BASE - PERIPH_BASE)
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

#define CSR_OFFSET               (RCC_OFFSET + 0x34)
#define CR_OFFSET                (PWR_OFFSET + 0x00)

#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

#define RTCEN_BitNumber          0x16
#define CSR_RTCEN_BB             (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (RTCEN_BitNumber * 4))

#define RTC_IT_ALRA              ((uint32_t)0x00001000)

void (*pFunc)(void);

void rtcInit() {
    __IO uint32_t initcounter = 0x00;
    uint32_t initstatus = 0x00;

    *(__IO uint32_t *) CR_DBP_BB = (uint32_t)ENABLE;
    *(__IO uint32_t *) CSR_RTCEN_BB = (uint32_t)ENABLE;

    // Configure the EXTI line connected internally to the RTC ALARM
    EXTI->PR    =  EXTI_PR_PR17; // Clear IT pending bit
    EXTI->IMR  |=  EXTI_IMR_MR17; // Enable interrupt request from EXTI line
    EXTI->EMR  &= ~EXTI_EMR_MR17 ; // Disable event on EXTI line
    EXTI->RTSR |=  EXTI_RTSR_TR17; // Trigger rising edge enabled
    EXTI->FTSR &= ~EXTI_FTSR_TR17; // Trigger falling edge disabled

    // Enable the RTC alarm IRQ
    NVIC_EnableIRQ(RTC_Alarm_IRQn);
    //NVIC_SetPriority(RTC_Alarm_IRQn, 4);

    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* Set the Initialization mode */
    RTC->ISR = (uint32_t)RTC_INIT_MASK;

    /* Wait till RTC is in INIT state and if Time out is reached exit */
    do {
      initstatus = RTC->ISR & RTC_ISR_INITF;
      initcounter++;
    } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

    /* Clear RTC CR FMT Bit */
    RTC->CR &= ((uint32_t)~(RTC_CR_FMT));
    /* Set RTC_CR register */
    //RTC->CR |=  ((uint32_t)(RTC_InitStruct->RTC_HourFormat));

    /* Configure the RTC PRER */
    RTC->PRER = (uint32_t)0xFF;
    RTC->PRER |= (uint32_t)((uint32_t)0x7F << 16);

    /* Exit Initialization mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF;
}

void rtcDeInit() {
    __IO uint32_t wutcounter = 0x00;
    uint32_t wutwfstatus = 0x00;
    __IO uint32_t synchrocounter = 0;
    uint32_t synchrostatus = 0x00;

    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* Reset TR, DR and CR registers */
    RTC->TR = (uint32_t)0x00000000;
    RTC->DR = (uint32_t)0x00002101;

    /* Reset All CR bits except CR[2:0] */
    RTC->CR &= (uint32_t)0x00000007;

    /* Wait till RTC WUTWF flag is set and if Time out is reached exit */
    do {
      wutwfstatus = RTC->ISR & RTC_ISR_WUTWF;
      wutcounter++;
    } while((wutcounter != INITMODE_TIMEOUT) && (wutwfstatus == 0x00));

    if ((RTC->ISR & RTC_ISR_WUTWF) == RESET) {
        return;
    } else {
        /* Reset all RTC CR register bits */
        RTC->CR        &= (uint32_t)0x00000000;
        RTC->WUTR      = (uint32_t)0x0000FFFF;
        RTC->PRER      = (uint32_t)0x007F00FF;
        RTC->CALIBR    = (uint32_t)0x00000000;
        RTC->ALRMAR    = (uint32_t)0x00000000;
        RTC->ALRMBR    = (uint32_t)0x00000000;
        RTC->SHIFTR    = (uint32_t)0x00000000;
        RTC->CALR       = (uint32_t)0x00000000;
        RTC->ALRMASSR  = (uint32_t)0x00000000;
        RTC->ALRMBSSR  = (uint32_t)0x00000000;

        /* Reset ISR register and exit initialization mode */
        RTC->ISR = (uint32_t)0x00000000;

        /* Reset Tamper and alternate functions configuration register */
        RTC->TAFCR = 0x00000000;

        /* Wait till the RTC RSF flag is set */
        /* Clear RSF flag */
        RTC->ISR &= (uint32_t)RTC_RSF_MASK;

        /* Wait the registers to be synchronised */
        do {
            synchrostatus = RTC->ISR & RTC_ISR_RSF;
            synchrocounter++;
        } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

        /* Enable the write protection for RTC registers */
        RTC->WPR = 0xFF;
    }
}

static uint8_t RTC_ByteToBcd2(uint8_t Value) {
  uint8_t bcdhigh = 0;

  while (Value >= 10) {
    bcdhigh++;
    Value -= 10;
  }

  return  ((uint8_t)(bcdhigh << 4) | Value);
}

void rtcSetAlarm(uint16_t timeSleep_sec, void (*func)(void)) {
    __IO uint32_t synchrocounter = 0;
    uint32_t synchrostatus = 0x00;
    __IO uint32_t initcounter = 0x00;
    uint32_t initstatus = 0x00;

    uint8_t s = 0;
    uint8_t m = 0;
    uint32_t tmpreg = 0;

    pFunc = func;

    PWR->CR |= PWR_CR_DBP; // Access to RTC, RTC Backup and RCC CSR registers enabled

    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /* Set the Initialization mode */
    RTC->ISR = (uint32_t)RTC_INIT_MASK;

    /* Wait till RTC is in INIT state and if Time out is reached exit */
    do
    {
      initstatus = RTC->ISR & RTC_ISR_INITF;
      initcounter++;
    } while((initcounter != INITMODE_TIMEOUT) && (initstatus == 0x00));

    /* Disable the Alarm in RTC_CR register */
    RTC->CR &= (uint32_t)~RTC_CR_ALRAE;

    /* Reset TR and DR registers */
    RTC->TR = (uint32_t)0x00000000;
    RTC->DR = (uint32_t)0x00002101;

    tmpreg = RTC_ALRMAR_MSK4 | RTC_ALRMAR_MSK3;

    m = timeSleep_sec / 60;
    s = timeSleep_sec - m * 60;

    m = RTC_ByteToBcd2(m);
    s = RTC_ByteToBcd2(s);

    tmpreg |= (m << 8) | s;

    RTC->ALRMAR = (uint32_t)tmpreg;

    /* Configure the Interrupts in the RTC_CR register */
    RTC->CR |= (uint32_t)(RTC_IT_ALRA & ~RTC_TAFCR_TAMPIE);
    /* Configure the Tamper Interrupt in the RTC_TAFCR */
    RTC->TAFCR |= (uint32_t)(RTC_IT_ALRA & RTC_TAFCR_TAMPIE);

    /* alarm A enable */
    RTC->CR |= RTC_CR_ALRAE;

    RTC->ISR &= ~RTC_ISR_ALRAF; // Clear the RTC alarm A flag

    /* Exit Initialization mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

    /* Wait till the RTC RSF flag is set */
    /* Clear RSF flag */
    RTC->ISR &= (uint32_t)RTC_RSF_MASK;

    /* Wait the registers to be synchronised */
    do {
        synchrostatus = RTC->ISR & RTC_ISR_RSF;
        synchrocounter++;
    } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFF;

    PWR->CR &= ~PWR_CR_DBP; // Access to RTC, RTC Backup and RCC CSR registers disabled
}

void RTC_Alarm_IRQHandler(void) {
    if (RTC->ISR & RTC_ISR_ALRAF) {
        // RTC alarm A interrupt
        (*pFunc)();
        PWR->CR  |= PWR_CR_DBP; // Access to RTC, RTC Backup and RCC CSR registers enabled
        RTC->ISR &= ~RTC_ISR_ALRAF; // Clear the RTC alarm A flag
        PWR->CR  &= ~PWR_CR_DBP; // Access to RTC, RTC Backup and RCC CSR registers disabled
        EXTI->PR = EXTI_PR_PR17; // Clear the EXTI pending bit
    }
}
