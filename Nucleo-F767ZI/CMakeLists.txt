cmake_minimum_required (VERSION 3.6)

if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
	set(SOS_TOOLCHAIN_CMAKE_PATH /Applications/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
	set(SOS_TOOLCHAIN_CMAKE_PATH C:/StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()
if( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
	set(SOS_TOOLCHAIN_CMAKE_PATH /StratifyLabs-SDK/Tools/gcc/arm-none-eabi/cmake)
endif()

include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-variables.cmake)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-sdk.cmake)

set(CMAKE_TOOLCHAIN_FILE ${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp-toolchain.cmake)
get_filename_component(SOS_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

#Add sources to the project
sos_sdk_add_out_of_source_directory(SOS_KERNEL_SOURCELIST ../src src)
list(APPEND SOS_KERNEL_SOURCELIST
	${CMAKE_SOURCE_DIR}/src/sl_config.h
	${CMAKE_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_SOURCE_DIR}/sl_settings.json
	)

#Change to boot sources
sos_sdk_add_out_of_source_directory(SOS_BOOT_SOURCELIST ../boot boot)
list(APPEND SOS_BOOT_SOURCELIST
	${CMAKE_SOURCE_DIR}/src/sl_config.h
	${CMAKE_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_SOURCE_DIR}/sl_settings.json
	)

set(SOS_ARCH v7em_f5dh)
set(SOS_DEVICE stm32f767zi)
set(SOS_DEVICE_FAMILY stm32f767xx)
set(SOS_HARDWARD_ID 0x00000014)
set(SOS_LIBRARIES arm_dsp_sos_kernel mbedtls_kernel)
set(SOS_INCLUDE_DIRECTORIES ${TOOLCHAIN_INC_DIR}/mcu/arch/stm32/cmsis)

set(SOS_KERNEL_START_ADDRESS 0x08100000)
set(SOS_BOOT_START_ADDRESS 0x08000000)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp-std.cmake)

