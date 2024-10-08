#ifndef SYSTEM
#define SYSTEM

#include "tm1650.h"
#include "i2c.h"

#define STM32G030xx 
#include "stm32g0xx.h"

uint32_t SystemCoreClock;

void SysTick_Init(void);
void SystemClock_Config(void);
void UART2_SendNumber(int number);
void UART2_SendString(const char *str);
void UART2_SendChar(uint8_t data);
void UART2_Init(void);

void GPIOA_Init(void);
void PA4_Output_Init(void);
void PA4_Set(void);
void PA4_Reset(void);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif // SYSTEM