cmake_minimum_required (VERSION 3.19)

project(Nucleo144-SDK)

set(API_IS_SDK TRUE CACHE INTERNAL "Build SDK projects")
option(IS_PULL "Pull SDK projects" OFF)

set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
file(MAKE_DIRECTORY ${BASE_DIRECTORY})

message(STATUS "Base directory ${BASE_DIRECTORY}")

#if INSTALL=ON the library will be staged to be part of an application SDK deployment

set(GIT_STATUS_DEPENDS "")

add_custom_target(pull_sdk)

macro(sdk_add_subdirectory NAME BRANCH INSTALL)

	if(IS_PULL OR NOT EXISTS ${BASE_DIRECTORY}/${NAME})
		sos_sdk_git_clone_or_pull_branch(${BASE_DIRECTORY} ${NAME} ssh://git@github.com/StratifyLabs/${NAME}.git ${BRANCH})
	endif()

	if(${INSTALL})
		message(STATUS "Adding ${NAME} to install target")
		add_subdirectory(${BASE_DIRECTORY}/${NAME} ${NAME})
	else()
		message(STATUS "Excluding ${NAME} from install target")
		add_subdirectory(${BASE_DIRECTORY}/${NAME} ${NAME} EXCLUDE_FROM_ALL)
	endif()

	add_custom_target(pull_${NAME}
		COMMAND git pull
		WORKING_DIRECTORY ${BASE_DIRECTORY}/${NAME}
		)
	add_dependencies(pull_sdk pull_${NAME})
endmacro()

set(STRATIFYOS_LWIP_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/InetAPI/lwip/include
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/InetAPI/lwip/lwip-2.1.2/src/include
	CACHE INTERNAL "StratifyOS LWIP INCLUDE PATHS")

set(LWIP_STRATIFYOS_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/dependencies/StratifyOS/include
	CACHE INTERNAL "LWIP StratifyOS INCLUDE PATHS")

set(API_IS_SDK ON CACHE INTERNAL "API_IS_SDK disable include statemenets")

sdk_add_subdirectory(API main ON)
include(${BASE_DIRECTORY}/API/libraries/API.cmake)

set(MBEDTLS_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/config/mbedtls/config.h CACHE INTERNAL "MBEDTLS config file")
set(LWIP_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/config CACHE INTERNAL "LWIP config path")
set(SOS_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/config CACHE INTERNAL "StratifyOS config directory")
set(STM32_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/config CACHE INTERNAL "STM32 config path")

set(STRATIFYOS_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/StratifyOS/include CACHE INTERNAL "Stratify OS include directory")

set(BUILD_STM32F767 ON CACHE INTERNAL "ENABLE BUILDING OF F767")
set(BUILD_STM32F446 ON CACHE INTERNAL "ENABLE BUILDING OF F446")
set(BUILD_STM32F746 ON CACHE INTERNAL "ENABLE BUILDING OF F746")
set(BUILD_STM32_ALL OFF CACHE INTERNAL "DONT BUILD ALL STM32 BUILDS")

sdk_add_subdirectory(StratifyOS-compiler-rt v1.0 OFF)
sdk_add_subdirectory(StratifyOS-newlib v1.0 OFF)
sdk_add_subdirectory(StratifyOS main ON)
sdk_add_subdirectory(StratifyOS-mcu-stm32 main OFF)
sdk_add_subdirectory(HalAPI main ON)
sdk_add_subdirectory(SosAPI main ON)
sdk_add_subdirectory(JsonAPI main ON)
sdk_add_subdirectory(CryptoAPI main ON)
sdk_add_subdirectory(InetAPI main ON)
sdk_add_subdirectory(CloudAPI main ON)



add_custom_target(status DEPENDS ${GIT_STATUS_DEPENDS})
