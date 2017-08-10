#include "stm32l1xx.h"
#include "gpio.h"
#include "i2c.h"

void I2CInit(I2C_t *obj)
{
    register uint32_t gpio_reg;

    I2C_TypeDef *i2c = (I2C_TypeDef *)(obj->I2C);

    /* Gpio settings */
    GpioInit(&obj->Scl, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0);
    GpioInit(&obj->Sda, PIN_ALTERNATE_FCT, PIN_OPEN_DRAIN, PIN_NO_PULL, 0);
    
    /* Gpio alternate function settings */
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Scl.portIndex)))->AFR[(obj->Scl.pinIndex < 8)?0:1] |= 0x04 << 4*(obj->Scl.pinIndex - ((obj->Scl.pinIndex < 8)?0:8));
    ((GPIO_TypeDef *)(AHBPERIPH_BASE + 0x400*(obj->Sda.portIndex)))->AFR[(obj->Sda.pinIndex < 8)?0:1] |= 0x04 << 4*(obj->Sda.pinIndex - ((obj->Sda.pinIndex < 8)?0:8));

    /* Enable bus clock */
    if(i2c == I2C1) {
        /* RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; */
        *((uint32_t *)(0x42000000+(32*0x23820)+4*12)) = 0x01;
        /* bit banding address for SPI1 */
        gpio_reg = 0x42000000+(32*0x13000);
    } else {
        /* RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*15)) = 0x01;
        /* bit banding address for SPI3 */
        gpio_reg = 0x42000000+(32*0x3C00);
    }

    /* Settings SPI->CR1 */
    *((uint32_t *)(gpio_reg)) = (obj->cpha) & 0x01;
    *((uint32_t *)(gpio_reg + 4)) = (obj->cpol) & 0x01;
    *((uint32_t *)(gpio_reg + 8)) = !(obj->slave_on);

    *((uint32_t *)(gpio_reg + 12)) = (obj->f_pclk) & 0x01;
    *((uint32_t *)(gpio_reg + 16)) = ((obj->f_pclk) >> 1) & 0x01;
    *((uint32_t *)(gpio_reg + 20)) = ((obj->f_pclk) >> 2) & 0x01;

    *((uint32_t *)(gpio_reg + 44)) = (obj->bits) & 0x01;
    *((uint32_t *)(gpio_reg + 28)) = (obj->msb_lsb) & 0x01;

    /* SPI Enable */
    /* SPI->CR1 set SPE */
    *((uint32_t *)(gpio_reg + 24)) = 0x01;
}

void I2CDeInit(I2C_t *obj)
{
    register uint32_t gpio_reg;

    I2C_TypeDef *i2c = (I2C_TypeDef *)(obj->I2C);

    /* Disable I2C */
    if(i2c == I2C1) {
        /* bit banding address for SPI1 */
        gpio_reg = 0x42000000+(32*0x13000);
        *((uint32_t *)(gpio_reg + 24)) = 0x00;
        /* RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN; */
        *((uint32_t *)(0x42000000+(32*0x23820)+4*12)) = 0x00;
        
        RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;
    } else {
        /* bit banding address for SPI3 */
        gpio_reg = 0x42000000+(32*0x3C00);
        *((uint32_t *)(gpio_reg + 24)) = 0x00;
        /* RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN; */
        *((uint32_t *)(0x42000000+(32*0x23824)+4*15)) = 0x00;
        
        RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN;
    }

    GpioDeInit(&obj->Scl);
    GpioDeInit(&obj->Sda);
}
