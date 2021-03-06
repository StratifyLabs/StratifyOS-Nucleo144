

macro(nucleo_target
		BOOT_START_ADDRESS
		BOOT_SOURCELIST
		BOOT_LINKER_FILE
		BOOT_LIBRARIES
		KERNEL_START_ADDRESS
		KERNEL_SOURCELIST
		KERNEL_LINKER_FILE
		KERNEL_LIBRARIES
		DEFINITIONS
		HARDWARE_ID
		ARCH)

sos_sdk_bsp_target(RELEASE ${PROJECT_NAME} "" release ${ARCH})
sos_sdk_bsp_target(DEBUG ${PROJECT_NAME} "" debug ${ARCH})

add_executable(${RELEASE_TARGET})

target_sources(${RELEASE_TARGET}
	PUBLIC
	${KERNEL_SOURCELIST}
	)
target_include_directories(${RELEASE_TARGET}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)
target_compile_definitions(${RELEASE_TARGET}
	PUBLIC
	MCU_ARCH_STM32
	__BOOT_START_ADDRESS=${BOOT_START_ADDRESS}
	__KERNEL_START_ADDRESS=${KERNEL_START_ADDRESS}
	${DEFINITIONS}
	)
target_compile_options(${RELEASE_TARGET} PUBLIC -Os)
target_link_directories(${RELEASE_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts
	)
set_target_properties(${RELEASE_TARGET}
	PROPERTIES
	LINK_FLAGS
	"-u symbols_table -T${KERNEL_LINKER_FILE}"
	)

add_executable(${DEBUG_TARGET})
sos_sdk_copy_target(${RELEASE_TARGET} ${DEBUG_TARGET})

sos_sdk_bsp_target(BOOT_RELEASE ${PROJECT_NAME} boot release ${ARCH})
sos_sdk_bsp_target(BOOT_DEBUG ${PROJECT_NAME} boot debug ${ARCH})

add_executable(${BOOT_RELEASE_TARGET})

target_sources(${BOOT_RELEASE_TARGET}
	PUBLIC
	${NUCLEO_BOOT_SOURCELIST}
	)

target_include_directories(${BOOT_RELEASE_TARGET}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../src
	${CMAKE_CURRENT_SOURCE_DIR}/src
	)

target_link_directories(${BOOT_RELEASE_TARGET}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts
	)

target_compile_definitions(${BOOT_RELEASE_TARGET}
	PUBLIC
	${DEFINITIONS}
	__BOOT_START_ADDRESS=${BOOT_START_ADDRESS}
	__KERNEL_START_ADDRESS=${KERNEL_START_ADDRESS}
	_IS_BOOT=1
	)

target_compile_options(${BOOT_RELEASE_TARGET} PUBLIC -Os)
set_target_properties(${BOOT_RELEASE_TARGET}
	PROPERTIES
	LINK_FLAGS
	"-u _main -T${BOOT_LINKER_FILE} "
	)


add_executable(${BOOT_DEBUG_TARGET})
sos_sdk_copy_target(${BOOT_RELEASE_TARGET} ${BOOT_DEBUG_TARGET})

sos_sdk_bsp("${BOOT_RELEASE_OPTIONS}" ${HARDWARE_ID} ${BOOT_START_ADDRESS} "${BOOT_LIBRARIES}")
sos_sdk_bsp("${BOOT_DEBUG_OPTIONS}" ${HARDWARE_ID} ${BOOT_START_ADDRESS} "${BOOT_LIBRARIES}")

sos_sdk_bsp("${RELEASE_OPTIONS}" ${HARDWARE_ID} ${KERNEL_START_ADDRESS} "${KERNEL_LIBRARIES}")
sos_sdk_bsp("${DEBUG_OPTIONS}" ${HARDWARE_ID} ${KERNEL_START_ADDRESS} "${KERNEL_LIBRARIES}")

endmacro()
