#ifndef __I2C_H__
#define __I2C_H__

typedef enum
{
  I2C_STANDART_SPEED  = 0, // 100 kHz
  I2C_FAST_SPEED           // 400 kHz
}I2C_speed;

typedef struct I2C_t
{
  uint32_t *I2C; //I2C1 or I2C2
  Gpio_t Scl;
  Gpio_t Sda;

  I2C_speed speed;
} I2C_t;


void I2CInit(I2C_t *obj);

void I2CDeInit(I2C_t *obj);



#endif  // __I2C_H__
