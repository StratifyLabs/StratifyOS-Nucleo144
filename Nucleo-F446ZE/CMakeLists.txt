cmake_minimum_required (VERSION 3.12)

if(NOT SOS_SDK_PATH)
	set(SOS_SDK_PATH $ENV{SOS_SDK_PATH})
endif()

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)
include(sos-sdk)

include(${CMAKE_CURRENT_SOURCE_DIR}/../nucleo.cmake)

set(ARCH v7em_f4sh)

include(compiler_rt)
include(newlib)
include(StratifyOS)
include(jansson)
include(StratifyOS_stm32f446xx_debug_${ARCH})
include(StratifyOS_stm32f446xx_release_${ARCH})

get_filename_component(SOS_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

set(NUCLEO_KERNEL_START_ADDRESS 0x08040000)
set(NUCLEO_BOOT_START_ADDRESS 0x08000000)
set(NUCLEO_HARDWARE_ID 0x00000008)
set(NUCLEO_DEFINITIONS
	SOS_BOARD_SYSTEM_CLOCK=168000000
	SYSTEM_MEMORY_SIZE=32768
	)
set(NUCLEO_LINKER_FILE ldscripts/stm32f446ze-rom.ld)
set(NUCLEO_MCU_LIBRARY StratifyOS_stm32f446xx)


#Add sources to the project list
sos_sdk_add_out_of_source_directory(NUCLEO_KERNEL_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../src src)
list(APPEND NUCLEO_KERNEL_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	)

#Change to boot sources list
sos_sdk_add_out_of_source_directory(NUCLEO_BOOT_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../boot boot)
list(APPEND NUCLEO_BOOT_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	)

set(BOOT_LIBRARIES StratifyOS_stm32f446xx StratifyOS_boot StratifyOS_usbd)
set(KERNEL_LIBRARIES
	StratifyOS_stm32f446xx
	StratifyOS_sys
	StratifyOS_usbd
	newlib_libc
	newlib_libm
	StratifyOS_sys)

nucleo_target(
	${NUCLEO_BOOT_START_ADDRESS} "${NUCLEO_BOOT_SOURCELIST}" ${NUCLEO_LINKER_FILE} "${BOOT_LIBRARIES}"
	${NUCLEO_KERNEL_START_ADDRESS} "${NUCLEO_KERNEL_SOURCELIST}" ${NUCLEO_LINKER_FILE} "${KERNEL_LIBRARIES}"
	"${NUCLEO_DEFINITIONS}"
	${NUCLEO_HARDWARE_ID}
	${ARCH}
)



