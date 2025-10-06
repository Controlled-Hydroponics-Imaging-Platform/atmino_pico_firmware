# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICOTOOL_FETCH_FROM_GIT_PATH "${CMAKE_SOURCE_DIR}/.pico-deps" CACHE PATH "Shared picotool path")

# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PhotoCon C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(lib/pico_captive_connect)
add_subdirectory(lib/pico_common_sensors_lib)

## Rest of the project

# header files
# include_directories(include) #out dated style

# Add executable. Default name is the project name, version 0.1
add_executable( ${PROJECT_NAME} 
        src/main.cpp

)

pico_set_program_name(${PROJECT_NAME} "PhotoCon")
pico_set_program_version(${PROJECT_NAME} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
        pico_captive_connect
        pico_common_sensors_lib
        )

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR} #root
        ${CMAKE_CURRENT_LIST_DIR}/include #/include

) #new style

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME} 
        
        )

pico_add_extra_outputs(${PROJECT_NAME})

