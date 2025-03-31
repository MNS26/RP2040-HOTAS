add_library(iniConfig INTERFACE)
# Add the standard include files to the build
target_include_directories(iniConfig INTERFACE
src # for our common or any other standard includes, if required
)

target_sources(iniConfig INTERFACE
${CMAKE_CURRENT_LIST_DIR}/src/iniConfig-old.cpp
)