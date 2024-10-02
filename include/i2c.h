#ifndef _I2C_
#define _I2C_

#include "stm32g030xx.h"

void I2C2_Init(void);
void sendData(I2C_TypeDef *I2C, uint32_t i2cAddress, uint8_t *data,
              uint8_t size);

#endif // _I2C_
