cmake_minimum_required (VERSION 3.12)

include(${CMAKE_CURRENT_SOURCE_DIR}/../nucleo.cmake)

set(ARCH v7em_f4sh)

add_custom_target(publish_header_F412ZG
	COMMAND sl os.publish:path=Nucleo-F412ZG,header
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	)

add_custom_target(publish_F412ZG
	COMMAND sl os.publish:path=Nucleo-F412ZG,signkey=RhnmvxQ8D4tlh02L8693
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	)

get_filename_component(SOS_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${SOS_NAME} CXX C)

set(NUCLEO_KERNEL_START_ADDRESS 0x080C0000)
set(NUCLEO_BOOT_START_ADDRESS 0x08000000)
set(NUCLEO_HARDWARE_ID 0x00000011)
set(NUCLEO_DEFINITIONS
	SOS_BOARD_SYSTEM_CLOCK=96000000
	SYSTEM_MEMORY_SIZE=32768
	)
set(NUCLEO_LINKER_FILE stm32f412zg-rom.ld)
set(NUCLEO_MCU_LIBRARY StratifyOS_stm32f412zx)


#Add sources to the project list
cmsdk_add_out_of_source_directory(NUCLEO_KERNEL_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../src src)
list(APPEND NUCLEO_KERNEL_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32-common.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f4xx.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f412zg-rom.ld
	)

#Change to boot sources list
cmsdk_add_out_of_source_directory(NUCLEO_BOOT_SOURCELIST ${CMAKE_CURRENT_SOURCE_DIR}/../boot boot)
list(APPEND NUCLEO_BOOT_SOURCELIST
	${CMAKE_CURRENT_SOURCE_DIR}/src/sl_config.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/SystemClock_Config.c
	${CMAKE_CURRENT_SOURCE_DIR}/sl_settings.json
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32-common.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f4xx.ld
	${CMAKE_CURRENT_SOURCE_DIR}/ldscripts/stm32f412zg-rom.ld
	)

set(BOOT_LIBRARIES tinycrypt_kernel uECC_kernel StratifyOS_stm32f412zx StratifyOS_boot StratifyOS_usbd)
set(KERNEL_LIBRARIES
	tinycrypt_kernel
	uECC_kernel
	StratifyOS_stm32f412zx
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



