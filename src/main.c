#include "system.h"

int main(void) {
  SysTick_Init();
  SystemClock_Config(); // Speed up core from 16MHz to 64MHz
  UART2_Init();

  GPIOA_Init();
  I2C2_Init();
  PA4_Output_Init();

  tm1650_init(I2C2, 3);
  while (1) {
    PA4_Set();
    tm1650_clearDisplay();

    delay_ms(1000);
    tm1650_sendText("324");
    tm1650_highlight();
    PA4_Reset();
    tm1650_sendTextMoving("Hello  boy   ", 450);
  }
}

void GPIOA_Init(void) {
  // Enable clock for GPIOA (bit 0 in RCC_IOPENR register)
  RCC->IOPENR |= (1 << 0);
}

void PA4_Output_Init(void) {
  // Set PA4 as output mode (01: General purpose output mode)
  GPIOA->MODER &= ~(3 << (4 * 2)); // Clear the MODER bits for PA4
  GPIOA->MODER |= (1 << (4 * 2));  // Set the MODER bits for PA4 to 01
}

void PA4_Set(void) {
  // Set PA4 (set bit 4 in BSRR register)
  GPIOA->BSRR = (1 << 4);
}

void PA4_Reset(void) {
  // Reset PA4 (set bit 20 in BSRR register)
  GPIOA->BSRR = (1 << (4 + 16));
}
