#include "system.h"

void SystemClock_Config(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;
  PWR->CR1 |= PWR_CR1_VOS_0;

  // 1. Enable HSI and wait until it is ready
  RCC->CR |= RCC_CR_HSION; // Enable HSI
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;

  // 2. Configure PLL
  RCC->PLLCFGR =
      (RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLPEN |
       RCC_PLLCFGR_PLLREN            // Set HSI as the PLL source
       | (8 << RCC_PLLCFGR_PLLN_Pos) // Set the PLL multiplier to achieve 64 MHz
                                     // (HSI = 16 MHz * 8 / 2)
       | (1 << RCC_PLLCFGR_PLLR_Pos)); // Set the division factor for PLLR
                                       // (divide by 2)

  // 2. Turn PLL on
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (2 << FLASH_ACR_LATENCY_Pos);
  RCC->CFGR &= ~RCC_CFGR_SW;

  // 3. Switch System Clock to PLL
  RCC->CFGR |= RCC_CFGR_SW_1;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;

  SystemCoreClockUpdate();
}

// uint32_t AHBPrescTable[16UL] = {0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 1UL, 2UL, 3UL, 4UL, 6UL, 7UL, 8UL, 9UL};
// uint32_t APBPrescTable[8UL] =  {0UL, 0UL, 0UL, 0UL, 1UL, 2UL, 3UL, 4UL};

// #define READ_BIT(REG, BIT)    ((REG) & (BIT))

#if !defined  (HSE_VALUE)
#define HSE_VALUE    (8000000UL)    /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE  (16000000UL)   /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

#if !defined  (LSI_VALUE)
 #define LSI_VALUE   (32000UL)     /*!< Value of LSI in Hz*/
#endif /* LSI_VALUE */

#if !defined  (LSE_VALUE)
  #define LSE_VALUE  (32768UL)      /*!< Value of LSE in Hz*/
#endif /* LSE_VALUE */


/************************* Miscellaneous Configuration ************************/
/* Note: Following vector table addresses must be defined in line with linker
         configuration. */
/*!< Uncomment the following line if you need to relocate the vector table
     anywhere in Flash or Sram, else the vector table is kept at the automatic
     remap of boot address selected */
/* #define USER_VECT_TAB_ADDRESS */

#if defined(USER_VECT_TAB_ADDRESS)
/*!< Uncomment the following line if you need to relocate your vector Table
     in Sram else user remap will be done in Flash. */
/* #define VECT_TAB_SRAM */
#if defined(VECT_TAB_SRAM)
#define VECT_TAB_BASE_ADDRESS   SRAM_BASE       /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#else
#define VECT_TAB_BASE_ADDRESS   FLASH_BASE      /*!< Vector Table base address field.
                                                     This value must be a multiple of 0x200. */
#define VECT_TAB_OFFSET         0x00000000U     /*!< Vector Table base offset field.
                                                     This value must be a multiple of 0x200. */
#endif /* VECT_TAB_SRAM */
#endif /* USER_VECT_TAB_ADDRESS */

// void SystemCoreClockUpdate(void)
// {
//   uint32_t tmp;
//   uint32_t pllvco;
//   uint32_t pllr;
//   uint32_t pllsource;
//   uint32_t pllm;
//   uint32_t hsidiv;

//   /* Get SYSCLK source -------------------------------------------------------*/
//   switch (RCC->CFGR & RCC_CFGR_SWS)
//   {
//     case RCC_CFGR_SWS_0:                /* HSE used as system clock */
//       SystemCoreClock = HSE_VALUE;
//       break;

//     case (RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0):  /* LSI used as system clock */
//       SystemCoreClock = LSI_VALUE;
//       break;

//     case RCC_CFGR_SWS_2:                /* LSE used as system clock */
//       SystemCoreClock = LSE_VALUE;
//       break;

//     case RCC_CFGR_SWS_1:  /* PLL used as system clock */
//       /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
//          SYSCLK = PLL_VCO / PLLR
//          */
//       pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
//       pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1UL;

//       if(pllsource == 0x03UL)           /* HSE used as PLL clock source */
//       {
//         pllvco = (HSE_VALUE / pllm);
//       }
//       else                              /* HSI used as PLL clock source */
//       {
//           pllvco = (HSI_VALUE / pllm);
//       }
//       pllvco = pllvco * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
//       pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos) + 1UL);

//       SystemCoreClock = pllvco/pllr;
//       break;
      
//     case 0x00000000U:                   /* HSI used as system clock */
//     default:                            /* HSI used as system clock */
//       hsidiv = (1UL << ((READ_BIT(RCC->CR, RCC_CR_HSIDIV))>> RCC_CR_HSIDIV_Pos));
//       SystemCoreClock = (HSI_VALUE/hsidiv);
//       break;
//   }
//   /* Compute HCLK clock frequency --------------------------------------------*/
//   /* Get HCLK prescaler */
//   tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos)];
//   /* HCLK clock frequency */
//   SystemCoreClock >>= tmp;
// }

void UART2_Init(void) {
  // USART_TypeDef *uart1 = USART2;
  //  Enable clock for GPIOA and USART2
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;    // Enable GPIOA clock
  RCC->APBENR1 |= RCC_APBENR1_USART2EN; // Enable USART2 clock

  // Configure PA2 as USART2_TX (AF1)
  GPIOA->MODER &= ~GPIO_MODER_MODE2;  // Clear mode
  GPIOA->MODER |= GPIO_MODER_MODE2_1; // Set PA2 to Alternate Function mode
  GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL2_Pos); // Set AF1 for PA2

  // Configure USART2
  USART2->BRR = SystemCoreClock / 115200; // Set baud rate to 115200
  USART2->CR1 = USART_CR1_TE;             // Enable transmitter
  USART2->CR1 |= USART_CR1_UE;            // Enable USART2
}

void UART2_SendChar(uint8_t data) {
  // Wait until TXE flag is set (TX buffer is empty)
  while (!(USART2->ISR & USART_ISR_TXE_TXFNF))
    ;

  // Send data
  USART2->TDR = data;
}

void UART2_SendString(const char *str) {
  while (*str) {
    UART2_SendChar(*str++);
  }
}

void UART2_SendNumber(int number) {
  char buffer[12]; // Buffer to hold the string representation of the number
                   // (enough for 32-bit int)
  int index = 0;

  // Handle negative numbers
  if (number < 0) {
    UART2_SendChar('-'); // Send the negative sign
    number = -number;    // Convert to positive
  }

  // Convert the number to string (manual approach)
  do {
    buffer[index++] =
        (number % 10) + '0'; // Get the last digit and convert to ASCII
    number /= 10;            // Remove the last digit
  } while (number > 0);

  // Send the string in reverse order
  while (index > 0) {
    UART2_SendChar(buffer[--index]);
  }
}

void SysTick_Init(void) {
  // Load the SysTick Reload Value Register to 16000 - 1 for 1 ms delay (16 MHz
  // / 16000 = 1 kHz)
  SysTick->LOAD = (64000000 / 1000000) - 1;
  // Set the SysTick Current Value Register to 0
  SysTick->VAL = 0;
  // Enable SysTick, use the processor clock, and enable interrupt
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void delay_us(uint32_t us) {
  for (uint32_t i = 0; i < us; i++) {
    // Wait until the COUNTFLAG is set
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk))
      ;
  }
}

void delay_ms(uint32_t ms) { delay_us(ms * 1000); }
