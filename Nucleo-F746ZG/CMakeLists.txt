cmake_minimum_required (VERSION 3.12)

include(${CMAKE_CURRENT_SOURCE_DIR}/../nucleo.cmake)

set(ARCH v7em_f5sh)

set(STM32_CHIP_FAMILY stm32f746xx)
set(STM32_CHIP stm32f746zg)

add_custom_target(publish_header_F746ZG
	COMMAND sl os.publish:path=Nucleo-F746ZG,header
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	)

add_custom_target(publish_F746ZG
	COMMAND sl os.publish:path=Nucleo-F746ZG,signkey=RhnmvxQ8D4tlh02L8693
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	)

get_filename_component(SOS_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

set(NUCLEO_KERNEL_START_ADDRESS 0x08060000)
set(NUCLEO_BOOT_START_ADDRESS 0x08000000)
set(NUCLEO_HARDWARE_ID 0x00000013)
set(NUCLEO_DEFINITIONS
	SOS_BOARD_SYSTEM_CLOCK=216000000
	SYSTEM_MEMORY_SIZE=65536
	INCLUDE_ETHERNET=1
	INCLUDE_JANSSON_API=1
	INCLUDE_TLS=1
	F746ZG=1
	)
set(NUCLEO_LINKER_FILE ${STM32_CHIP}-rom.ld)
set(NUCLEO_MCU_LIBRARY StratifyOS_${STM32_CHIP_FAMILY})


#Add sources to the project list
cmsdk_add_out_of_source_directory(NUCLEO_KERNEL_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../src src)
cmsdk_add_out_of_source_directory(NUCLEO_ETHERNET_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../src/ethernet ethernet)
list(APPEND NUCLEO_KERNEL_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/${STM32_CHIP}-rom.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f746xg-rom.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32-common.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f7xx.ld
	${NUCLEO_ETHERNET_SOURCELIST}
	)

#Change to boot sources list
cmsdk_add_out_of_source_directory(NUCLEO_BOOT_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../boot boot)
list(APPEND NUCLEO_BOOT_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	)

set(BOOT_LIBRARIES ${NUCLEO_MCU_LIBRARY}
	tinycrypt_kernel
	uECC_kernel
	StratifyOS_boot
	StratifyOS_usbd)

set(KERNEL_LIBRARIES
	tinycrypt_kernel
	uECC_kernel
	jansson_kernel
	mbedtls_kernel
	lwip
	${NUCLEO_MCU_LIBRARY}
	StratifyOS_sys
	StratifyOS_usbd
	newlib_libc
	newlib_libm
	StratifyOS_sys
	)

nucleo_target(
	BOOT_START_ADDRESS ${NUCLEO_BOOT_START_ADDRESS}
	BOOT_SOURCELIST ${NUCLEO_BOOT_SOURCELIST}
	BOOT_LINKER_FILE ${NUCLEO_LINKER_FILE}
	BOOT_LIBRARIES ${BOOT_LIBRARIES}
	KERNEL_START_ADDRESS ${NUCLEO_KERNEL_START_ADDRESS}
	KERNEL_SOURCELIST ${NUCLEO_KERNEL_SOURCELIST}
	KERNEL_LINKER_FILE ${NUCLEO_LINKER_FILE}
	KERNEL_LIBRARIES ${KERNEL_LIBRARIES}
	DEFINITIONS ${NUCLEO_DEFINITIONS}
	HARDWARE_ID ${NUCLEO_HARDWARE_ID}
	ARCH ${ARCH})


