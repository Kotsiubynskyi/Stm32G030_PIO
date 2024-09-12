This a PlatformIO (PIO) project for STM32G030F6P6 using CMSIS only.
It's a demonstration of how to work with ultra cheap LED segment display IC TM1650
Author used STM32G030 board from WeActStudio (can be found on AliExpress)

Unfortunately PIO uses outdated stm32flash version which knows nothing about relatively new STM32G030 MCU. To be able to upload firmware to STM32G030 please update stm32flash manually to newer version and put here:
C:\Users\{UserName}\.platformio\packages\tool-stm32duino\stm32flash

Latest stm32flash can be found here:
https://sourceforge.net/projects/stm32flash/


Upload firmware to your STM32G030:
* In platformio.ini file change line with COM port to your COM port
* run command `C:\Users\{USER}\.platformio\penv\Scripts\platformio.exe run --target upload`



# Setup for LazyVim
* install `clangd` LSP server in LazyVim Mason plugin manager
* run `cmake .` to generate compile_command.json in root directory
* nvim restart may be needed
* open any `c` file


# Compile .elf binary (invalid for now)
* Install Ninja: `sudo apt install ninja-build -y`
* `cmake -G Ninja .` 
* `ninja`
