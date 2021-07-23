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

set(SOS_ARCH v7em_f4sh)
set(SOS_ARCH_LIST "v7em_f5sh")

add_subdirectory(SDK)
add_subdirectory(Nucleo-F446ZE)
#add_subdirectory(Nucleo-F746ZG)
#add_subdirectory(Nucleo-F767ZI)
