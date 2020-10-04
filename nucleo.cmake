sos_sdk_bsp_target(RELEASE ${SOS_NAME} "" release v7em_f4sh)
sos_sdk_bsp_target(DEBUG ${SOS_NAME} "" debug v7em_f4sh)

add_executable(${RELEASE_TARGET})
target_sources(${RELEASE_TARGET}
	PUBLIC
	${NUCLEO_KERNEL_SOURCELIST}
	)
target_include_directories(${RELEASE_TARGET}
	PUBLIC
	${CMAKE_SOURCE_DIR}/../src
	${CMAKE_SOURCE_DIR}/src
	)
target_compile_definitions(${RELEASE_TARGET}
	PUBLIC
	MCU_ARCH_STM32
	${NUCLEO_DEFINITIONS}
	)
target_compile_options(${RELEASE_TARGET} PUBLIC -Os)
set_target_properties(${RELEASE_TARGET}
	PROPERTIES
	LINK_FLAGS
	"-u symbols_table -T${NUCLEO_LINKER_FILE}"
	)

add_executable(${DEBUG_TARGET})
sos_sdk_copy_target(${RELEASE_TARGET} ${DEBUG_TARGET})

target_link_libraries(${RELEASE_TARGET}
	StratifyOS_sys
	${NUCLEO_MCU_LIBRARY}
	m
	c
	StratifyOS_sys
	${NUCLEO_MCU_LIBRARY}
	gcc-hard
	)

target_link_libraries(${DEBUG_TARGET}
	StratifyOS_sys_debug
	${NUCLEO_MCU_LIBRARY}_debug
	c
	m
	StratifyOS_sys_debug
	${NUCLEO_MCU_LIBRARY}_debug
	gcc-hard
	)

sos_sdk_bsp("${RELEASE_OPTIONS}" 0x00000008 0x08040000)
sos_sdk_bsp("${DEBUG_OPTIONS}" 0x00000008 0x08040000)

sos_sdk_bsp_target(BOOT_RELEASE ${SOS_NAME} boot release v7em_f4sh)
sos_sdk_bsp_target(BOOT_DEBUG ${SOS_NAME} boot debug v7em_f4sh)
add_executable(${BOOT_RELEASE_TARGET})
target_sources(${BOOT_RELEASE_TARGET}
	PUBLIC
	${NUCLEO_BOOT_SOURCELIST}
	)
target_include_directories(${BOOT_RELEASE_TARGET}
	PUBLIC
	${CMAKE_SOURCE_DIR}/../src
	${CMAKE_SOURCE_DIR}/src
	)
target_compile_definitions(${BOOT_RELEASE_TARGET}
	PUBLIC
	MCU_ARCH_STM32
	${NUCLEO_DEFINITIONS}
	__KERNEL_START_ADDRESS=${NUCLEO_KERNEL_START_ADDRESS}
	)
target_compile_options(${BOOT_RELEASE_TARGET} PUBLIC -Os)
set_target_properties(${BOOT_RELEASE_TARGET}
	PROPERTIES
	LINK_FLAGS
	"-u _main -T${NUCLEO_LINKER_FILE} "
	)


add_executable(${BOOT_DEBUG_TARGET})
sos_sdk_copy_target(${BOOT_RELEASE_TARGET} ${BOOT_DEBUG_TARGET})

target_link_libraries(${BOOT_RELEASE_TARGET}
	StratifyOS_sys
	StratifyOS_boot
	${NUCLEO_MCU_LIBRARY}
	m
	c
	StratifyOS_sys
	StratifyOS_boot
	${NUCLEO_MCU_LIBRARY}
	gcc-hard
	)

target_link_libraries(${BOOT_DEBUG_TARGET}
	StratifyOS_sys_debug
	StratifyOS_boot_debug
	${NUCLEO_MCU_LIBRARY}_debug
	m
	c
	StratifyOS_sys_debug
	StratifyOS_boot_debug
	${NUCLEO_MCU_LIBRARY}_debug
	gcc-hard
	)


sos_sdk_bsp("${BOOT_RELEASE_OPTIONS}" ${NUCLEO_HARDWARE_ID} ${NUCLEO_BOOT_START_ADDRESS})
sos_sdk_bsp("${BOOT_DEBUG_OPTIONS}" ${NUCLEO_HARDWARE_ID} ${NUCLEO_BOOT_START_ADDRESS})
