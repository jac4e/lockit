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
enable_language(C ASM)

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
set(TOTP_DIR lib/TOTP-MCU)

#############
# MCU Setup #
#############
set(MCU STM32L432xx)
set(MCU_FAMILY STM32L4xx)
set(MCU_DIR inc/${MCU_FAMILY})
set(MCU_SPEC cortex-m4)
set(FLOAT_SPEC "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
string(TOLOWER startup_${MCU}.s STARTUP_FILE_NAME)
string(TOLOWER system_${MCU_FAMILY}.c SYSTEM_FILE_NAME)
set(STARTUP_FILE ${MCU_DIR}/Source/Templates/gcc/${STARTUP_FILE_NAME})
set(SYSTEM_FILE ${MCU_DIR}/Source/Templates/${SYSTEM_FILE_NAME})
set(HSE_VAL 8000000)
set(LINKER_SCRIPT STM32L432KCUX_FLASH.ld)

message(STATUS "[INFO] Cross compiling for ${MCU}")
message(STATUS "[INFO] Startup file used is ${STARTUP_FILE}")
message(STATUS "[INFO] Startup file used is ${SYSTEM_FILE}")

#################
# General Setup #
#################

set(OBJECT_GEN_FLAGS " \
-fno-builtin \
-Wall \
-ffunction-sections -fdata-sections \
-fomit-frame-pointer \
-mcpu=${MCU_SPEC} \
-mthumb \
-mthumb-interwork \
-mabi=aapcs \
${FLOAT_SPEC} \
" CACHE INTERNAL "Common flags for C/CXX/ASM/Linker")

################
# Linker Setup #
################

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map -Wl,--print-memory-usage -Wl,--gc-sections ${OBJECT_GEN_FLAGS}")

# Linker script
configure_file(${LINKER_SCRIPT} ${LINKER_SCRIPT} COPYONLY)

##################
# Compiler Setup #
##################


# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g ${OBJECT_GEN_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ${OBJECT_GEN_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -g ${OBJECT_GEN_FLAGS}")

# Cross-compiler setup
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_C_GDB gdb)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

# Files to be compiled

# if(TARGET_GROUP STREQUAL release)
    # HAL Library
    file(GLOB HAL_SRC ${HAL_DIR}/Src/*.c)
    add_library(HAL STATIC ${HAL_SRC})
    target_include_directories(
        HAL
        PUBLIC
            ${HAL_DIR}/Inc
            ${MCU_DIR}/Include
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
    list(FILTER TOTP_SRC EXCLUDE REGEX "${TOTP_DIR}/blink.c")
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
            # SEMIHOSTING=${SEMIHOSTING}
    )

    # Flash target
    add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
    string(TOLOWER target/${MCU_FAMILY}.cfg FLASH_CONFIG_FILE_NAME)
    # need to make sure above string onle has one x before cfg
    string(REPLACE "xx" "x" FLASH_CONFIG_FILE_NAME ${FLASH_CONFIG_FILE_NAME})
    add_custom_command(
        TARGET flash
        COMMAND echo "Flashing ${PROJECT_NAME}.elf"
        COMMAND openocd -f interface/stlink.cfg -f ${FLASH_CONFIG_FILE_NAME} -c "program ${PROJECT_NAME}.elf verify reset exit"
    )
# else()
#     message(FATAL_ERROR "Invalid target group")
# endif()
