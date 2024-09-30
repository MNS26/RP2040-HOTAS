add_library(usb_util INTERFACE)
# Add the standard include files to the build
target_include_directories(usb_util INTERFACE
src # for our common lwipopts or any other standard includes, if required
)

target_sources(usb_util INTERFACE
${CMAKE_CURRENT_LIST_DIR}/src/usbd_interface.cpp
${CMAKE_CURRENT_LIST_DIR}/src/usbd_device.cpp
${CMAKE_CURRENT_LIST_DIR}/src/usbd_cdc.cpp
${CMAKE_CURRENT_LIST_DIR}/src/usbd_hid.cpp
${CMAKE_CURRENT_LIST_DIR}/src/usbd_msc.cpp
${CMAKE_CURRENT_LIST_DIR}/src/usbd_mtp.cpp

)