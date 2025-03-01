cmake_minimum_required(VERSION 3.10.0)

#################
# Project Setup #
#################

project(lockit)
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   1   CACHE STRING "Project patch version number.")
set(VERSION_STRING  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" CACHE STRING "Project version string.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_STRING)

#################
# Library Setup #
#################

# HAL Setup
set(HAL_DIR lib/stm32l4xx-hal-driver)

# RTOS Setup
set(RTOS_DIR lib/FreeRTOS-Kernel)
set(RTOS_PORT_DIR lib/FreeRTOS-Kernel/portable/GCC/ARM_CM4F)
set(RTOS_HEAP lib/FreeRTOS-Kernel/portable/MemMang/heap_4.c)

# CMSIS Setup
set(CMSIS_DIR lib/CMSIS/CMSIS)

# TOTP-MCU Setup
set(TOTP_DIR lib/totp-mcu)

#############
# MCU Setup #
#############
set(MCU STM32L432xx)
set(MCU_FAMILY STM32L4xx)
set(MCU_DIR inc/${MCU_FAMILY})
set(MCU_SPEC cortex-m4)
set(FLOAT_SPECC "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
string(TOLOWER startup_${MCU}.s STARTUP_FILE_NAME)
set(STARTUP_FILE ${MCU_DIR}/${STARTUP_FILE_NAME})
set(SYSTEM_FILE ${MCU_DIR}/system_${MCU}.c)
set(HSE_VAL 8000000)
set(LINKER_SCRIPT linker.ld)



# if(TARGET_GROUP STREQUAL release)
    # HAL Library
    file(GLOB HAL_SRC ${HAL_DIR}/Src/*.c)
    add_library(HAL STATIC ${HAL_SRC})
    target_include_directories(
        HAL
        PUBLIC
            ${HAL_DIR}/Inc
            ${CMSIS_DIR}/Core/Include
        PRIVATE
            ${PROJECT_SOURCE_DIR}/inc
    )
    target_compile_definitions(
        HAL
        PUBLIC
        ${MCU}
        HSE_VALUE=${HSE_VAL}
    )

    # RTOS Library
    file(GLOB RTOS_SRC ${RTOS_DIR}/*.c ${RTOS_PORT_DIR}/*.c ${RTOS_HEAP})
    add_library(RTOS STATIC ${RTOS_SRC})
    target_include_directories(
        RTOS
        PUBLIC
            ${RTOS_DIR}/include
            ${RTOS_PORT_DIR}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/inc
    )

    # TOTP Library
    file(GLOB TOTP_SRC ${TOTP_DIR}/*.c)
    add_library(TOTP STATIC ${TOTP_SRC})
    target_include_directories(
        TOTP
        PUBLIC
            ${TOTP_DIR}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/inc
    )

    # Main
    file(GLOB_RECURSE MAIN_SRC src/*.c)
    set(SOURCES ${MAIN_SRC} ${STARTUP_FILE} ${SYSTEM_FILE})
    add_executable(${PROJECT_NAME}.elf ${SOURCES})
    target_include_directories(
        ${PROJECT_NAME}.elf
        PUBLIC
            ${PROJECT_SOURCE_DIR}/inc
    )
    target_link_libraries(
        ${PROJECT_NAME}.elf
        PUBLIC
            HAL
            RTOS
            TOTP
    )
    target_compile_definitions(
        ${PROJECT_NAME}.elf
        PUBLIC
            ${MCU}
            HSE_VALUE=${HSE_VAL}
            SEMIHOSTING=${SEMIHOSTING}
    )

    # Flash target
    add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
    add_custom_command(
        TARGET flash
        COMMAND echo "Starting GDB client and loading ${PROJECT_NAME}.elf dashboard to $(GDB_TTY)"
        COMMAND ${CMAKE_C_GDB} ${PROJECT_NAME}.elf 
        -ex "target extended :3333"
        -ex "dashboard -output $(GDB_TTY)"
        -ex "load ${PROJECT_NAME}.elf"
        -ex "monitor arm semihosting enable"
    )
# else()
#     message(FATAL_ERROR "Invalid target group")
# endif()
