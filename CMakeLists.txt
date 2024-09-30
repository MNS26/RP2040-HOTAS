# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)


# Pull in FreeRTOS
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/freertos)
include(FreeRTOS_Kernel_import.cmake)

#target_compile_definitions(${PROJECT_NAME} PRIVATE
#    FREE_RTOS_KERNEL_SMP=1
#    #configNUMBER_OF_CORES=1
#    )

project(HOTAS C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PUBLIC
${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_interface.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_device.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_cdc.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_hid.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_msc.cpp
#${CMAKE_CURRENT_LIST_DIR}/src/usbd_mtp.cpp

)

pico_set_program_name(${PROJECT_NAME} "HOTAS")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Generate PIO header
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

add_subdirectory(lib/usb build/usb)
add_subdirectory(lib/sd_card build/sd)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
    pico_async_context_freertos
    FreeRTOS-Kernel-Heap4
    pico_stdlib
)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_LIST_DIR}/
${CMAKE_CURRENT_LIST_DIR}/lib
${CMAKE_CURRENT_LIST_DIR}/src
${CMAKE_CURRENT_LIST_DIR}/include # for our common lwipopts or any other standard includes, if required
${FREERTOS_KERNEL_PATH}/include
${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/include/
)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME} 
        hardware_spi
        hardware_i2c
        hardware_pio
        hardware_timer
        hardware_clocks
        pico_unique_id 
        pico_multicore
        tinyusb_device 
        tinyusb_board
        usb_util
        #FREERTOS_PORT                               # FreeRTOS config files
#        sd_util
        )

pico_add_extra_outputs(${PROJECT_NAME})

