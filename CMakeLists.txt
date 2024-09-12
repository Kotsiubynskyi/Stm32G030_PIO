cmake_minimum_required(VERSION 3.15)
project(STM32G0_PIO C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(DEVICE_FAMILY STM32G0)
set(MCU_MODEL STM32G030xx)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMSIS_PATH ${CMAKE_SOURCE_DIR}/include)
include_directories(
    ${CMSIS_PATH}
)

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/ldscript_g0.ld)
add_executable(${PROJECT_NAME}.elf ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}.elf m)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -Wl,--gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0plus -mthumb -Wall -Werror -Wno-error")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-m0plus -mthumb -Wall -Werror -Wno-error")

add_custom_target(print_size
    ALL
    arm-none-eabi-size ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME}.elf
)
