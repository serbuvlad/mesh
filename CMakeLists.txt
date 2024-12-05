# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(mesh C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(
  mesh

  mesh.c
  screen/st7735.c
  screen/font.c
  screen/screen.c
  keyboard/keyboard.c
  keyboard/keyboard2.c
)

pico_set_program_name(mesh "mesh")
pico_set_program_version(mesh "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(mesh 0)
pico_enable_stdio_usb(mesh 1)

pico_generate_pio_header(
  ${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/screen/spi3wire.pio
)

# Add the standard library to the build
target_link_libraries(mesh
  pico_stdlib
  hardware_pio
)

# Add the standard include files to the build
target_include_directories(mesh PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(mesh)

