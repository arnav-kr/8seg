# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)


project(8seg C CXX ASM)

# Add project root as an include directory for config.hpp
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add subdirectories for libraries
add_subdirectory(lib/Driver)
add_subdirectory(lib/WiFi)
add_subdirectory(lib/Clock)
add_subdirectory(lib/Fs)
add_subdirectory(lib/AsyncTimer)
add_subdirectory(lib/StateManager)

# Add executable. Default name is the project name, version 0.1

add_executable(8seg 8seg.cpp)

pico_set_program_name(8seg "8seg")
pico_set_program_version(8seg "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(8seg 0)
pico_enable_stdio_usb(8seg 1)

# Add the standard library to the build
target_link_libraries(8seg
    pico_stdlib)

# Add the standard include files to the build
target_include_directories(8seg PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
# Link our driver library too
target_link_libraries(8seg
    driver
    wifi
    clockmod
    fs
    asynctimer
    statemanager
)

pico_add_extra_outputs(8seg)

