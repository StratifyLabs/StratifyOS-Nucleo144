

function(nucleo_target)
  set(OPTIONS "")
  set(PREFIX ARGS)
  set(ONE_VALUE_ARGS
    BOOT_START_ADDRESS
    BOOT_LINKER_FILE
    KERNEL_START_ADDRESS
    KERNEL_LINKER_FILE
    ARCH
    HARDWARE_ID)
  set(MULTI_VALUE_ARGS
    BOOT_SOURCELIST
    BOOT_LIBRARIES
    KERNEL_SOURCELIST
    KERNEL_LIBRARIES
    DEFINITIONS)
  cmake_parse_arguments(PARSE_ARGV 0 ${PREFIX} "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}")

  foreach(VALUE ${ONE_VALUE_ARGS})
    if(NOT ARGS_${VALUE})
      message(FATAL_ERROR "nucleo_target requires ${VALUE}")
    endif()
  endforeach()
  foreach(VALUE ${MULTI_VALUE_ARGS})
    if(NOT ARGS_${VALUE})
      message(FATAL_ERROR "nucleo_target requires ${VALUE}")
    endif()
  endforeach()

  cmsdk2_bsp_add_executable(
    NAME ${PROJECT_NAME}
    CONFIG release
    SUFFIX .elf
    ARCH ${ARGS_ARCH}
    TARGET RELEASE_TARGET)
  cmsdk2_bsp_add_executable(
    NAME ${PROJECT_NAME}
    CONFIG debug
    SUFFIX .elf
    ARCH ${ARGS_ARCH}
    TARGET DEBUG_TARGET)

  target_sources(${RELEASE_TARGET}
    PUBLIC
    ${ARGS_KERNEL_SOURCELIST}
    )
  target_include_directories(${RELEASE_TARGET}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
  target_compile_definitions(${RELEASE_TARGET}
    PUBLIC
    MCU_ARCH_STM32
    __BOOT_START_ADDRESS=${ARGS_BOOT_START_ADDRESS}
    __KERNEL_START_ADDRESS=${ARGS_KERNEL_START_ADDRESS}
    ${ARGS_DEFINITIONS}
    )
  target_compile_options(${RELEASE_TARGET} PUBLIC -Os)
  target_link_directories(${RELEASE_TARGET}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts
    )
  set_target_properties(${RELEASE_TARGET}
    PROPERTIES
    LINK_FLAGS
    "-u symbols_table -T${ARGS_KERNEL_LINKER_FILE}")

  cmsdk2_copy_target(
    SOURCE ${RELEASE_TARGET}
    DESTINATION ${DEBUG_TARGET})

  cmsdk2_bsp_add_executable(
    NAME ${PROJECT_NAME}
    OPTION boot
    CONFIG release
    SUFFIX .elf
    ARCH ${ARGS_ARCH}
    TARGET BOOT_RELEASE_TARGET)
  cmsdk2_bsp_add_executable(
    NAME ${PROJECT_NAME}
    OPTION boot
    CONFIG debug
    SUFFIX .elf
    ARCH ${ARGS_ARCH}
    TARGET BOOT_DEBUG_TARGET)

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
    ${ARGS_DEFINITIONS}
    __BOOT_START_ADDRESS=${ARGS_BOOT_START_ADDRESS}
    __KERNEL_START_ADDRESS=${ARGS_KERNEL_START_ADDRESS}
    _IS_BOOT=1
    )

  target_compile_options(${BOOT_RELEASE_TARGET} PUBLIC -Os)
  set_target_properties(${BOOT_RELEASE_TARGET}
    PROPERTIES
    LINK_FLAGS
    "-u _main -T${ARGS_BOOT_LINKER_FILE} "
    )

  cmsdk2_copy_target(
    SOURCE ${BOOT_RELEASE_TARGET}
    DESTINATION ${BOOT_DEBUG_TARGET})

  message(STATUS "Nucleo BOOT_START_ADDRESS ${ARGS_BOOT_START_ADDRESS}")
  message(STATUS "Nucleo BOOT_LIBRARIES ${ARGS_BOOT_LIBRARIES}")
  cmsdk2_bsp_add_dependencies(
    TARGET ${BOOT_RELEASE_TARGET}
    HARDWARE_ID ${ARGS_HARDWARE_ID}
    START_ADDRESS ${ARGS_BOOT_START_ADDRESS}
    DEPENDENCIES ${ARGS_BOOT_LIBRARIES})
  cmsdk2_bsp_add_dependencies(
    TARGET ${BOOT_DEBUG_TARGET}
    HARDWARE_ID ${ARGS_HARDWARE_ID}
    START_ADDRESS ${ARGS_BOOT_START_ADDRESS}
    DEPENDENCIES ${ARGS_BOOT_LIBRARIES})
  message(STATUS "Nucleo KERNEL_START_ADDRESS ${ARGS_KERNEL_START_ADDRESS}")
  message(STATUS "Nucleo KERNEL_LIBRARIES ${ARGS_KERNEL_LIBRARIES}")
  cmsdk2_bsp_add_dependencies(
    TARGET ${RELEASE_TARGET}
    HARDWARE_ID ${ARGS_HARDWARE_ID}
    START_ADDRESS ${ARGS_KERNEL_START_ADDRESS}
    DEPENDENCIES ${ARGS_KERNEL_LIBRARIES})
  cmsdk2_bsp_add_dependencies(
    TARGET ${DEBUG_TARGET}
    HARDWARE_ID ${ARGS_HARDWARE_ID}
    START_ADDRESS ${ARGS_KERNEL_START_ADDRESS}
    DEPENDENCIES ${ARGS_KERNEL_LIBRARIES})
endfunction()

