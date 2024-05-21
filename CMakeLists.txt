cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(ultrasonic
ultrasonic.c
)

target_link_libraries(ultrasonic
        pico_stdlib
        hardware_gpio
        hardware_timer
        pico_cyw43_arch_none
        )

# Enable USB output
pico_enable_stdio_usb(ultrasonic 1)
pico_enable_stdio_uart(ultrasonic 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ultrasonic)