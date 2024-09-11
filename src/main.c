#include "stm32g030xx.h"

int main(void)
{
    SysTick_Init();
    SystemClock_Config();
    UART2_Init();

    GPIOA_Init();
    PA4_Output_Init();
    while (1)
    {
        PA4_Set();
        delay(100);
        PA4_Reset();
        delay(100);
    }
}

void SystemClock_Config(void)
{
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    PWR->CR1 |= PWR_CR1_VOS_0;

    // 1. Enable HSI and wait until it is ready
    RCC->CR |= RCC_CR_HSION; // Enable HSI
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Configure PLL
    RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLREN // Set HSI as the PLL source
                    | (8 << RCC_PLLCFGR_PLLN_Pos)                                    // Set the PLL multiplier to achieve 64 MHz (HSI = 16 MHz * 8 / 2)
                    | (1 << RCC_PLLCFGR_PLLR_Pos));                                  // Set the division factor for PLLR (divide by 2)


    // 2. Turn PLL on
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (2 << FLASH_ACR_LATENCY_Pos);
    RCC->CFGR &= ~RCC_CFGR_SW;

    // 3. Switch System Clock to PLL
    RCC->CFGR |= RCC_CFGR_SW_1;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    SystemCoreClockUpdate();
}

void UART2_GPIO_Init(void)
{
    // Enable clock for GPIOA
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    // Configure PA2 as UART2_TX (Alternate Function mode)
    GPIOA->MODER &= ~(3 << (2 * 2));
    GPIOA->MODER |= (2 << (2 * 2));  // Set PA2 to Alternate Function mode
    GPIOA->AFR[0] |= (1 << (2 * 4)); // Set PA2 to AF1 (UART2_TX)

    // Configure PA3 as UART2_RX (Alternate Function mode)
    GPIOA->MODER &= ~(3 << (3 * 2));
    GPIOA->MODER |= (2 << (3 * 2));  // Set PA3 to Alternate Function mode
    GPIOA->AFR[0] |= (1 << (3 * 4)); // Set PA3 to AF1 (UART2_RX)
}

void UART2_Init(void)
{
    // Enable clock for GPIOA and USART2
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;    // Enable GPIOA clock
    RCC->APBENR1 |= RCC_APBENR1_USART2EN; // Enable USART2 clock

    // Configure PA2 as USART2_TX (AF1)
    GPIOA->MODER &= ~GPIO_MODER_MODE2;            // Clear mode
    GPIOA->MODER |= GPIO_MODER_MODE2_1;           // Set PA2 to Alternate Function mode
    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos); // Set AF1 for PA2

    // Configure USART2
    USART2->BRR = SystemCoreClock / 115200; // Set baud rate to 115200
    USART2->CR1 = USART_CR1_TE;             // Enable transmitter
    USART2->CR1 |= USART_CR1_UE;            // Enable USART2
}

void UART2_SendChar(uint8_t data)
{
    // Wait until TXE flag is set (TX buffer is empty)
    while (!(USART2->ISR & USART_ISR_TXE_TXFNF))
        ;

    // Send data
    USART2->TDR = data;
}

void UART2_SendString(const char *str)
{
    while (*str)
    {
        UART2_SendChar(*str++);
    }
}

void UART2_SendNumber(int number)
{
    char buffer[12]; // Buffer to hold the string representation of the number (enough for 32-bit int)
    int index = 0;

    // Handle negative numbers
    if (number < 0)
    {
        UART2_SendChar('-'); // Send the negative sign
        number = -number;    // Convert to positive
    }

    // Convert the number to string (manual approach)
    do
    {
        buffer[index++] = (number % 10) + '0'; // Get the last digit and convert to ASCII
        number /= 10;                          // Remove the last digit
    } while (number > 0);

    // Send the string in reverse order
    while (index > 0)
    {
        UART2_SendChar(buffer[--index]);
    }
}

void SysTick_Init(void)
{
    // Load the SysTick Reload Value Register to 16000 - 1 for 1 ms delay (16 MHz / 16000 = 1 kHz)
    SysTick->LOAD = 64000 - 1;
    // Set the SysTick Current Value Register to 0
    SysTick->VAL = 0;
    // Enable SysTick, use the processor clock, and enable interrupt
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void delay(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        // Wait until the COUNTFLAG is set
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
            ;
    }
}

void GPIOA_Init(void)
{
    // Enable clock for GPIOA (bit 0 in RCC_IOPENR register)
    RCC->IOPENR |= (1 << 0);
}

void PA4_Output_Init(void)
{
    // Set PA4 as output mode (01: General purpose output mode)
    GPIOA->MODER &= ~(3 << (4 * 2)); // Clear the MODER bits for PA4
    GPIOA->MODER |= (1 << (4 * 2));  // Set the MODER bits for PA4 to 01
}

void PA4_Set(void)
{
    // Set PA4 (set bit 4 in BSRR register)
    GPIOA->BSRR = (1 << 4);
}

void PA4_Reset(void)
{
    // Reset PA4 (set bit 20 in BSRR register)
    GPIOA->BSRR = (1 << (4 + 16));
}