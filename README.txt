This a PlatformIO (PIO) project for STM32G030F6P6 using CMSIS library.
Author used STM32G030 board from WeActStudio (can be found on AliExpress)

Unfortunately PIO uses outdated stm32flash version which knows nothing about relatively new STM32G030 MCU. To be able to upload firmware to STM32G030 please update stm32flash manually to newer version and put here:
C:\Users\{UserName}\.platformio\packages\tool-stm32duino\stm32flash

Latest stm32flash can be found here:
https://sourceforge.net/projects/stm32flash/