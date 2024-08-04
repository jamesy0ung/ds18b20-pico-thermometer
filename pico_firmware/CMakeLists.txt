cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(ds18b20_pico_thermometer C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()
add_executable(ds18b20_pico_thermometer
    main.c
)
target_link_libraries(ds18b20_pico_thermometer pico_stdlib hardware_gpio)
pico_enable_stdio_usb(ds18b20_pico_thermometer 1)
pico_enable_stdio_uart(ds18b20_pico_thermometer 0)
pico_add_extra_outputs(ds18b20_pico_thermometer) 