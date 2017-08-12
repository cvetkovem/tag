#include "stm32l1xx.h"
#include "gpio.h"
#include "i2c.h"

void I2C_init(I2C_t *obj)
{
    register uint32_t gpio_reg;

    I2C_TypeDef *i2c = (I2C_TypeDef *)(obj->I2C);

    /* Gpio settings */
    GpioInit(&obj->Scl, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0);
    GpioInit(&obj->Sda, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_PULL_UP, 0);

    /* Gpio alternate function settings */
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Scl.portIndex)))->AFR[(obj->Scl.pinIndex < 8)?0:1] |= 0x04 << 4*(obj->Scl.pinIndex - ((obj->Scl.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Sda.portIndex)))->AFR[(obj->Sda.pinIndex < 8)?0:1] |= 0x04 << 4*(obj->Sda.pinIndex - ((obj->Sda.pinIndex < 8)?0:8));

    /* Enable bus clock */
    if(i2c == I2C1) {
        /* RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*21)) = 0x01;
        /* bit banding address for I2C1 */
        gpio_reg = 0x42000000+(32*0x5400);
    } else {
        /* RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*22)) = 0x01;
        /* bit banding address for I2C2 */
        gpio_reg = 0x42000000+(32*0x5800);
    }

    /* Settings I2C */

    /* SWRST */
    *((uint32_t *)(gpio_reg + 4*15)) = 0x01;
    *((uint32_t *)(gpio_reg + 4*15)) = 0x00;

    /* I2C1->CR2 &= ~I2C_CR2_FREQ;
       I2C1->CR2 |= I2C_CR2_FREQ_5; */
    *((uint32_t *)(gpio_reg + 4*16*2 + 4*5)) = 0x01;

    /* I2C1->CCR &= ~I2C_CCR_CCR;
       I2C1->CCR |= 0xA0; */
    *((uint32_t *)(gpio_reg + 4*16*14 + 4*5)) = 0x01;
    *((uint32_t *)(gpio_reg + 4*16*14 + 4*7)) = 0x01;

    /* I2C1->TRISE = 0x21; */
    *((uint32_t *)(gpio_reg + 4*16*16 + 4*1)) = 0x00;
    *((uint32_t *)(gpio_reg + 4*16*16 + 4*0)) = 0x01;
    *((uint32_t *)(gpio_reg + 4*16*16 + 4*5)) = 0x01;

    /* I2C Enable */
    /* I2C_CR1_PE set PE */
    *((uint32_t *)(gpio_reg + 4*0)) = 0x01;

    /* I2C1->CR1 |= I2C_CR1_ACK; */
    *((uint32_t *)(gpio_reg + 4*10)) = 0x01;
}

void I2C_deInit(I2C_t *obj)
{
    register uint32_t gpio_reg;

    I2C_TypeDef *i2c = (I2C_TypeDef *)(obj->I2C);

    /* Disable I2C */
    if(i2c == I2C1) {
        /* bit banding address for I2C1 */
        gpio_reg = 0x42000000+(32*0x5400);
        *((uint32_t *)(gpio_reg + 4*0)) = 0x00; // I2C_CR1_PE
        /* RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*21)) = 0x00;
    } else {
        /* bit banding address for I2C2 */
        gpio_reg = 0x42000000+(32*0x5800);
        *((uint32_t *)(gpio_reg + 4*0)) = 0x00; // I2C_CR1_PE
        /* RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*22)) = 0x00;
    }

    GpioDeInit(&obj->Scl);
    GpioDeInit(&obj->Sda);
}

void I2C_burst_read(I2C_t *obj, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t *data) {
    register uint32_t gpio_reg;

    I2C_TypeDef *i2c = (I2C_TypeDef *)(obj->I2C);

    if(i2c == I2C1) {
        /* bit banding address for I2C1 */
        gpio_reg = 0x42000000+(32*0x5400);
    } else {
        /* bit banding address for I2C2 */
        gpio_reg = 0x42000000+(32*0x5800);
    }

	/* I2C_GenerateSTART(I2Cx, ENABLE);
	   I2C1->CR1 |= I2C_CR1_START; */
    *((uint32_t *)(gpio_reg + 4*8)) = 0x01;

    /* while (!(I2C1->SR1 & I2C_SR1_SB)); */
    while (!( i2c->SR1 & I2C_SR1_SB ));

    (void) i2c->SR1;

	/* I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Transmitter); */
	i2c->DR = HW_address & 0xFE; // 7bit addr + 0(write)

    /* while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); */
	while (!(i2c->SR1 & I2C_SR1_ADDR));

    (void) i2c->SR1;
    (void) i2c->SR2;

	/* I2C_SendData(I2Cx, addr); */
	i2c->DR = addr;

	/* while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); */
	while (!(i2c->SR1 & I2C_SR1_BTF));

	/* I2C_GenerateSTOP(I2Cx, ENABLE);
	   I2C1->CR1 |= I2C_CR1_STOP; */
    *((uint32_t *)(gpio_reg + 4*9)) = 0x01;

	/* I2C_GenerateSTART(I2Cx, ENABLE);
	   I2C1->CR1 |= I2C_CR1_START; */
    *((uint32_t *)(gpio_reg + 4*8)) = 0x01;

    /* while (!(I2C1->SR1 & I2C_SR1_SB)); */
    while (!( i2c->SR1 & I2C_SR1_SB ));

    (void) i2c->SR1;

	/* I2C_Send7bitAddress(I2Cx, HW_address, I2C_Direction_Receiver); */
	i2c->DR = (HW_address | 0x01); // 7bit addr + 1(read)

	/* while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); */
	while (!(i2c->SR1 & I2C_SR1_ADDR));

    (void) i2c->SR1;
	(void) i2c->SR2;

    while(n_data--) {
      while (!(i2c->SR1 & I2C_SR1_RXNE));
      (void) i2c->SR1;
	  (void) i2c->SR2;
      *data++ = i2c->DR;
	  }

    /* I2C1->CR1 &= ~I2C_CR1_ACK; */
    *((uint32_t *)(gpio_reg + 4*10)) = 0x00;

	/* I2C_GenerateSTOP(I2Cx, ENABLE);
	   I2C1->CR1 |= I2C_CR1_STOP; */
    *((uint32_t *)(gpio_reg + 4*9)) = 0x01;
}
