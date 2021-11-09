cmake_minimum_required(VERSION 3.12)

set(SDK_DIRECTORY ${CMAKE_SOURCE_DIR}/SDK)
set(SOS_SDK_PATH ${SDK_DIRECTORY}/StratifyLabs-SDK)

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)
include(sos-sdk)

project(StratifyOS-Nucleo144 CXX C ASM)
include(CTest)

# These options are mutually exclusive
option(F446ZE "Enable F446ZE build" OFF)
option(F746ZG "Enable F746ZG build" OFF)
option(F767ZI "Enable F767ZI build" OFF)

set(SOS_SDK_PULL_TARGET pull)
add_custom_target(pull
  COMMAND git pull
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


if(F446ZE)
  message(STATUS "Adding F446ZE")
  set(SOS_ARCH v7em_f4sh CACHE INTERNAL "Set Arch to v7em_f4sh for F446ZE")
elseif(F746ZG)
  message(STATUS "Adding F746ZG")
  set(SOS_ARCH v7em_f5sh CACHE INTERNAL "Set Arch to v7em_f5sh for F746ZG")
elseif(F767ZI)
  message(STATUS "Adding F767ZI")
  set(SOS_ARCH v7em_f5sh CACHE INTERNAL "Set Arch to v7em_f5sh for F767ZI")
else()
  message(FATAL_ERROR "Must specify one of `F446ZE` | `F746ZG` | `F767ZI`")
endif()

set(SOS_ARCH_LIST "" CACHE INTERNAL "No other architectures are enabled")
message(STATUS "SOS ARCH is ${SOS_ARCH}")

if(NOT F446ZE AND NOT F746ZG AND NOT F767ZI)
  message(FATAL_ERROR "Please specify Board")
endif()

add_custom_target(publish_header)
add_custom_target(publish)

add_subdirectory(SDK)
if(F446ZE)
  add_subdirectory(Nucleo-F446ZE)
  add_dependencies(publish_header publish_header_F446ZE)
  add_dependencies(publish publish_F446ZE)
endif()

if(F746ZG)
  add_subdirectory(Nucleo-F746ZG)
  add_dependencies(publish_header publish_header_F746ZG)
  add_dependencies(publish publish_F746ZG)
endif()

if(F767ZI)
  add_subdirectory(Nucleo-F767ZI)
  add_dependencies(publish_header publish_header_F767ZI)
  add_dependencies(publish publish_F767ZI)
endif()

add_subdirectory(apps)

add_custom_target(installOs
  COMMAND sl os.install:signkey=RhnmvxQ8D4tlh02L8693
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

