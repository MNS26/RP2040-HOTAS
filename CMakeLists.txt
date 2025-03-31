# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_DEOPTIMIZED_DEBUG 1)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")
#set(PICO_PLATFORM rp2040 CACHE STRING "Board platform")

set(PICO_TINYUSB_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/TinyUSB)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(HOTAS C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/hw_config.cpp
    src/usb_descriptors.cpp
    src/mtp_callbacks.cpp
)
include(${PICO_TINYUSB_PATH}/hw/bsp/rp2040/family.cmake)
include(${PICO_TINYUSB_PATH}/hw/bsp/family_support.cmake)
#include(lib/TinyUSB/hw/bsp/family_support.cmake)

pico_set_program_name(${PROJECT_NAME} "HOTAS")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Modify the below lines to enable/disable output over UART/USB/SEMIHOSTING
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_semihosting(${PROJECT_NAME} 1)


add_subdirectory(lib/no-OS-FatFS-SDIO-SPI/src FatFS_SPI)
#add_subdirectory(lib/iniConfig iniConfig)
#add_subdirectory(lib/yaml yaml)
#add_subdirectory(${PICO_TINYUSB_PATH}/src TinyUSB)
#add_subdirectory(lib/usb usb)



# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_LIST_DIR}
#${CMAKE_CURRENT_LIST_DIR}/lib
${CMAKE_CURRENT_LIST_DIR}/src
${CMAKE_CURRENT_LIST_DIR}/include
${PICO_TINYUSB_PATH}/src

${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)


# Add the standard library to the build
target_link_libraries(HOTAS
    pico_stdlib
    pico_util
    pico_unique_id
    pico_multicore
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_lwip_mdns
#    pico_lwip
#    pico_cyw43_arch_none
#    pico_cyw43_arch_lwip_threadsafe_background

#    hardware_adc
#    hardware_gpio
#    hardware_i2c
#    hardware_irq
#    hardware_pio
#    hardware_spi
#    hardware_timer
#    hardware_clocks

    tinyusb_common_base
    tinyusb_bsp
    tinyusb_device
    tinyusb_board

)

target_link_libraries(${PROJECT_NAME}
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
#    iniConfig
#    yaml
)
add_compile_definitions(USE_PRINTF USE_DBG_PRINTF)

pico_add_extra_outputs(${PROJECT_NAME})

