add_library(sd_util INTERFACE)
# Add the standard include files to the build
target_include_directories(sd_util INTERFACE
src # for our common lwipopts or any other standard includes, if required
)

target_sources(sd_util INTERFACE
${CMAKE_CURRENT_LIST_DIR}/src/FS/FS.cpp
${CMAKE_CURRENT_LIST_DIR}/src/SD/SD.cpp
${CMAKE_CURRENT_LIST_DIR}/src/SDFS/SDFS.cpp
${CMAKE_CURRENT_LIST_DIR}/src/SdFat/SdFat.cpp
${CMAKE_CURRENT_LIST_DIR}/src/
${CMAKE_CURRENT_LIST_DIR}/src/

)