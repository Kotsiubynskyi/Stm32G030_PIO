#include "tm1650.h"
#include "i2c.h"
#include "system.h"

uint8_t inited = 0;
uint8_t digits = 0;
uint8_t currentBrightness = 4;
I2C_TypeDef *I2C;

void tm1650_init(I2C_TypeDef *i2cArg, uint8_t digitsArg)
{
    if (inited == 1)
        return;

    I2C = i2cArg;
    digits = digitsArg;
    inited = 1;

    tm1650_clearDisplay();
}

void tm1650_sendSymbol(uint8_t position, uint8_t symbol)
{
    sendData(I2C2, 0x34 + position, &symbol, sizeof(symbol));
}

const uint8_t TM1650_CDigits[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00,
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53,
    0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F,
    0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08,
    0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C,
    0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00};

void tm1650_sendText(char *text)
{
    for (uint8_t position = 0; position < digits; position++)
    {
        uint8_t symbol = text[position];
        uint8_t value = TM1650_CDigits[symbol];
        tm1650_sendSymbol(position, value);
    }
}

void tm1650_sendTextMoving(char *text, uint32_t speed)
{
    tm1650_clearDisplay();
    uint8_t length = strlen(text);
    for (uint8_t position = 0; position < digits && position < length; position++)
    {
        uint8_t symbol = text[position];
        uint8_t value = TM1650_CDigits[symbol];
        tm1650_sendSymbol(position, value);
    }

    delay_ms(speed);
    if (length > digits)
    {
        for (uint8_t i = digits; i < length; i++)
        {
            for (uint8_t k = 0; k < digits; k++)
            {
                uint8_t symbol  = text[i - (digits - 1) + k];
                uint8_t value = TM1650_CDigits[symbol];
                tm1650_sendSymbol(k, value);
            }
            delay_ms(speed);
        }
    }
}

void tm1650_setMaxBrightness(void)
{
    if (!inited)
        return;

    tm1650_setBrightness(7);
}

void tm1650_setMinBrightness(void)
{
    if (!inited)
        return;

    tm1650_setBrightness(1);
}

void tm1650_setBrightness(uint8_t newLevel)
{
    if (!inited || newLevel > 7 || newLevel < 1)
        return;

    uint8_t newBrightness = (newLevel << TM1650_BRIGHT_SHIFT) | TM1650_ON;
    sendData(I2C2, 0x24, &newBrightness, 1);
    currentBrightness = newLevel;
}

void tm1650_highlight(void)
{
    uint8_t tempBrightness = currentBrightness;
    for (uint8_t i = 0; i < 3; i++)
    {
        for (uint8_t i = 1; i < 8; i++)
        {
            tm1650_setBrightness(i);
            delay_ms(50);
        }
        for (uint8_t i = 7; i > 0; i--)
        {
            tm1650_setBrightness(i);
            delay_ms(50);
        }
    }
    tm1650_setBrightness(tempBrightness);
}

void tm1650_clearDisplay(void)
{
    uint8_t clearCfg = (currentBrightness << TM1650_BRIGHT_SHIFT) | TM1650_ON;
    sendData(I2C, 0x24, &clearCfg, 1);

    for (uint8_t i = 0; i < digits; i++)
    {
        uint8_t clear[] = {0b00000000};
        sendData(I2C, 0x34 + i, clear, 1);
    }
}