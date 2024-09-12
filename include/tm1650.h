#ifndef TM1650
#define TM1650
#include "stm32g030xx.h"
#include <string.h>

#define TM1650_ON 0b00000001
#define TM1650_BRIGHT_SHIFT 4

void tm1650_init(I2C_TypeDef *I2C, uint8_t digits);
void tm1650_sendSymbol(uint8_t position, uint8_t symbol);
void tm1650_sendText(char *text);
void tm1650_sendTextMoving(char *text, uint32_t speed);
void tm1650_clearDisplay(void);
void tm1650_setMaxBrightness(void);
void tm1650_setMinBrightness(void);
void tm1650_setBrightness(uint8_t level);
void tm1650_highlight(void);

#endif // TM1650
