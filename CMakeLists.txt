cmake_minimum_required (VERSION 3.12)

set(SDK_DIRECTORY ${CMAKE_SOURCE_DIR}/SDK)
set(SOS_SDK_PATH ${SDK_DIRECTORY}/StratifyLabs-SDK)

set(ENV_SOS_SDK_PATH $ENV{SOS_SDK_PATH})

message(STATUS "ENV SOS_SDK_PATH is ${ENV_SOS_SDK_PATH}")

if(NOT ${ENV_SOS_SDK_PATH} STREQUAL ${SOS_SDK_PATH})
	message(FATAL_ERROR "ENV SOS_SDK_PATH must be ${SOS_SDK_PATH}")
endif()

set(ENV{SOS_SDK_PATH} ${SOS_SDK_PATH})

set(CMAKE_MODULE_PATH ${SOS_SDK_PATH}/cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/sos-toolchain.cmake)
include(sos-sdk)

project(StratifyOS-Nucleo144 CXX C ASM)
include(CTest)

option(F446ZE "Enable F446ZE build" OFF)
option(F746ZG "Enable F746ZG build" OFF)
option(F767ZI "Enable F767ZI build" OFF)

set(SOS_ARCH "")
set(SOS_ARCH_LIST "")

if(F446ZE)
	message(STATUS "Adding F446ZE")
	set(SOS_ARCH v7em_f4sh)
endif()

if(F746ZG)
	message(STATUS "Adding F746ZG")
	if(NOT SOS_ARCH)
		set(SOS_ARCH v7em_f5sh)
	else()
		set(SOS_ARCH_LIST ${SOS_ARCH_LIST} v7em_f5sh)
	endif()
endif()

if(F767ZI)
	message(STATUS "Adding F767ZI")
	if(NOT SOS_ARCH)
		set(SOS_ARCH v7em_f5dh)
	else()
		set(SOS_ARCH_LIST ${SOS_ARCH_LIST} v7em_f5dh)
	endif()
endif()

message(STATUS "SOS ARCH is ${SOS_ARCH}")
message(STATUS "SOS EXTRA ARCH is ${SOS_ARCH_LIST}")

if(NOT F446ZE AND NOT F746ZG AND NOT F767ZI)
	message(FATAL_ERROR "Please specify Board")
endif()


add_subdirectory(SDK)
if(F446ZE)
	add_subdirectory(Nucleo-F446ZE)
endif()

if(F746ZG)
	add_subdirectory(Nucleo-F746ZG)
endif()

if(F767ZI)
	add_subdirectory(Nucleo-F767ZI)
endif()



add_subdirectory(apps)

add_custom_target(pull
	COMMAND git pull
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS pull_apps pull_sdk
	)
