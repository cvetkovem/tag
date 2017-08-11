#ifndef __I2C_H__
#define __I2C_H__

/** 32MHz STANDART MODE(100KHz) MASTER I2C **/

typedef struct I2C_t
{
  uint32_t *I2C; //I2C1 or I2C2
  Gpio_t Scl;
  Gpio_t Sda;
} I2C_t;


void I2C_init(I2C_t *obj);

void I2C_deInit(I2C_t *obj);

void I2C_burst_read(I2C_t *obj, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t *data);

#endif  // __I2C_H__
