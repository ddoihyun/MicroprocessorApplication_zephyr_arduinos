cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(CO2_LCD_example)

target_sources(app PRIVATE
    src/main.c
    src/LCD.c
    src/uart_co2.c
)

target_include_directories(app PRIVATE include)