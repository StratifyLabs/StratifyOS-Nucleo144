cmake_minimum_required (VERSION 3.6)

if(NOT SOS_SDK_PATH)
	set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
endif()

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/Tools/gcc/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)

include(sos-sdk)

get_filename_component(SOS_NAME ${CMAKE_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

set(NUCLEO_KERNEL_START_ADDRESS 0x08040000)
set(NUCLEO_BOOT_START_ADDRESS 0x08000000)
set(NUCLEO_HARDWARE_ID 0x00000008)
set(NUCLEO_DEFINITIONS
	__stm32f446xx
	STM32F446xx=1
	SOS_BOARD_SYSTEM_CLOCK=168000000
	)
set(NUCLEO_LINKER_FILE ldscripts/stm32f446ze-rom.ld)
set(NUCLEO_MCU_LIBRARY StratifyOS_stm32f446xx)


#Add sources to the project list
sos_sdk_add_out_of_source_directory(NUCLEO_KERNEL_SOURCELIST ${CMAKE_SOURCE_DIR}/../src src)
list(APPEND NUCLEO_KERNEL_SOURCELIST
	${CMAKE_SOURCE_DIR}/src/sl_config.h
	${CMAKE_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_SOURCE_DIR}/sl_settings.json
	)

#Change to boot sources list
sos_sdk_add_out_of_source_directory(NUCLEO_BOOT_SOURCELIST ${CMAKE_SOURCE_DIR}/../boot boot)
list(APPEND NUCLEO_BOOT_SOURCELIST
	${CMAKE_SOURCE_DIR}/src/sl_config.h
	${CMAKE_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_SOURCE_DIR}/sl_settings.json
	)

include(${CMAKE_SOURCE_DIR}/../nucleo.cmake)
