set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.30)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(MSPM0G3507 C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(
        -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft
        -ffunction-sections -fdata-sections
        -Og -g
)

add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Og -g)
endif ()

include_directories(
        ${CMAKE_SOURCE_DIR}/mspm0-sdk

        ${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib
        ${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib/m0p
        ${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib/m0p/sysctl
        ${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/devices/msp/m0p
        ${CMAKE_SOURCE_DIR}/mspm0-sdk/third_party/CMSIS/Core/Include
)

add_definitions(-D__MSPM0G3507__)

file(GLOB DRIVERLIB_COMMON
        "${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib/dl_*.c"
        "${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib/m0p/*.c"
        "${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/driverlib/m0p/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c"
)

set(STARTUP_FILE
        "${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/devices/msp/m0p/startup_system_files/gcc/startup_mspm0g350x_gcc.c"
)

set(SOURCES
    Core/Src/main.c
    Core/Src/syscall.c
    ${DRIVERLIB_COMMON}
    ${STARTUP_FILE}
)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/mspm0-sdk/ti/devices/msp/m0p/linker_files/gcc/mspm0g3507.lds)
add_link_options(
        -specs=nosys.specs
        -Wl,--no-warn-rwx-segments
        -Wl,-gc-sections
        -Wl,--print-memory-usage
        -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
        -mcpu=cortex-m0plus
        -mthumb
        -mthumb-interwork
        -T ${LINKER_SCRIPT}
)
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
