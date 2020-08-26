cmake_minimum_required (VERSION 3.6)

# Grab the SDK location
if(NOT DEFINED SOS_SDK_PATH)
	if(DEFINED ENV{SOS_SDK_PATH})
		set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH /Applications/StratifyLabs-SDK)
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH C:/StratifyLabs-SDK)
	elseif( ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
		message(WARNING "default SDK locations are deprecated")
		set(SOS_SDK_PATH /StratifyLabs-SDK)
	endif()
endif()
if(NOT DEFINED SOS_SDK_PATH)
	message(FATAL_ERROR "Please set environment variable SOS_SDK_PATH to location of the StratifyLabs-SDK directory")
endif()
set(SOS_TOOLCHAIN_CMAKE_PATH ${SOS_SDK_PATH}/Tools/gcc/arm-none-eabi/cmake)
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
sos_sdk_add_out_of_source_directory(SOS_KERNEL_SOURCELIST ../src/ethernet src/ethernet)
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
set(SOS_DEVICE stm32h743zi)
set(SOS_DEVICE_FAMILY stm32h743xx)
set(SOS_HARDWARD_ID 0x00000021)
set(SOS_LIBRARIES	mbedtls_kernel lwip_kernel jansson_kernel)
set(SOS_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/src ${TOOLCHAIN_INC_DIR}/mcu/arch/stm32/cmsis)
set(SOS_DEFINITIONS
	SOS_BOARD_SYSTEM_CLOCK=480000000
	SOS_BOARD_FLAGS=SYS_FLAG_IS_ACTIVE_ON_IDLE
	INCLUDE_JANSSON_API=1
	)
set(SOS_KERNEL_START_ADDRESS 0x08100000)
set(SOS_BOOT_START_ADDRESS 0x08000000)
include(${SOS_TOOLCHAIN_CMAKE_PATH}/sos-bsp-std.cmake)
