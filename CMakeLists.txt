cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME icarus-firmware)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # General utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/time.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/utils/volatile.cxx
    # Sensors
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sensors/adxl375.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sensors/lis2mdl.cxx 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sensors/icm-45686.cxx 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/sensors/ms5607.cxx
    # GPS
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ublox/parser.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/ublox/gps.cxx
    # Radio
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/radio/message.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/radio/parser.cxx
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
