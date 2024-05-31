cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(MainApp
main.c
        )
target_link_libraries(MainApp
        pico_stdlib
        hardware_gpio
        hardware_timer
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
)
# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_BOARD pico_w CACHE STRING "Board type")
# Pull in Raspberry Pi Pico SDK (must be before project)
if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
        message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()
# Initialise the Raspberry Pi Pico SDK
pico_set_program_name(MainApp "MainApp")
pico_set_program_version(MainApp "1.0")
pico_enable_stdio_uart(MainApp 1)
pico_enable_stdio_usb(MainApp 1)
target_include_directories(MainApp PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)
pico_add_extra_outputs(MainApp)