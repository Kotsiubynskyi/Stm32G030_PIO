#include "i2c.h"
#include <stdint.h>

void I2C2_Init() {
  I2C_Manual_Reset();
  RCC->APBENR1 |= RCC_APBENR1_I2C2EN; // enable I2C2 clock
  RCC->AHBENR |= RCC_IOPENR_GPIOAEN;

  GPIOA->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12); // Clear mode bits
  GPIOA->MODER |=
      (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1); // Alternate function mode
  GPIOA->OTYPER |= (GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12);
  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED11 | GPIO_OSPEEDR_OSPEED12);

  GPIOA->AFR[1] &=
      ~(GPIO_AFRH_AFSEL11 | GPIO_AFRH_AFSEL12); // reset Alternate Function bits
  GPIOA->AFR[1] |=
      (6 << GPIO_AFRH_AFSEL11_Pos) |
      (6 << GPIO_AFRH_AFSEL12_Pos); // Set Alternate Function AF4 tof PA11 and
                                    // PA12 to I2C SDA and CLK

  RCC->APBRSTR1 |= RCC_APBRSTR1_I2C2RST;
  RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2C2RST;
}

void I2C_Manual_Reset(void) {
  RCC->APBENR1 |= RCC_IOPENR_GPIOAEN;

  // Configure PA11 (SCL) and PA12 (SDA) as general-purpose output
  GPIOA->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12); // Clear mode bits
  GPIOA->MODER |=
      (GPIO_MODER_MODE11_0 | GPIO_MODER_MODE12_0); // set to normal mode

  // Set SCL and SDA as high (open-drain, pulled up externally)
  GPIOA->ODR |= (1 << 11) | (1 << 12); // SCL high, SDA high

  // Toggle SCL manually to generate 9 clock pulses to reset the bus
  for (int i = 0; i < 9; i++) {
    GPIOA->ODR &= ~(1 << 11); // SCL low
    for (volatile int j = 0; j < 10000; j++)
      ;                      // Small delay
    GPIOA->ODR |= (1 << 11); // SCL high
    for (volatile int j = 0; j < 10000; j++)
      ; // Small delay
  }
}

void sendData(I2C_TypeDef *I2C, uint32_t i2cAddress, uint8_t *data,
              uint8_t size) {
  while (I2C->CR2 & I2C_ISR_BUSY)
    ;

  I2C->CR1 &= ~(I2C_CR1_PE);    // disable I2C for configuring
  I2C->CR1 |= (I2C_CR1_ANFOFF); // enable analog filter
  I2C->CR1 |= (I2C_CR1_DNF);    // enable digital filter
  // I2C->TIMINGR = 0x10B17DB5;    // Configure timing register for 100kHz (this
  // value can be found in RefManual or calculated in CubeIDE)
  I2C->TIMINGR =
      0x2071A2FF; // Configure timing register for 50kHz (this value can be
                  // found in RefManual or calculated in CubeIDE)

  I2C->CR1 |= (I2C_CR1_PE);      // enable I2C for configuring
  I2C->CR2 &= ~(I2C_CR2_SADD);   // clear all address bits
  I2C->CR2 |= (i2cAddress << 1); // set address

  I2C->CR2 |= I2C_CR2_START | I2C_CR2_AUTOEND; // START
  I2C->CR2 &= ~(I2C_CR2_NBYTES);               // clear N-bytes register
  I2C->CR2 |= (size << I2C_CR2_NBYTES_Pos);    // set number of bytes to 1

  while (!(I2C->ISR & I2C_ISR_TXE))
    ;

  for (uint8_t i = 0; i < size; i++) {
    volatile uint8_t b = data[i];
    I2C->TXDR = b;

    while (!(I2C->ISR & I2C_ISR_TXE))
      ;
  }

  I2C->CR2 |= I2C_CR2_STOP;
  while (!(I2C->ISR & I2C_ISR_STOPF))
    ;
  I2C->ICR |= I2C_ICR_STOPCF;
}
