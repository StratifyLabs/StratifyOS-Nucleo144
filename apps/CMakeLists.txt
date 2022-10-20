cmake_minimum_required (VERSION 3.19)

project(StratifyOS-Nucleo144-Apps)

set(BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Base directory ${BASE_DIRECTORY}")

option(APPS_IS_TESTSUITE "Enable Test Suite" ON)
option(APPS_IS_DEMO "Build Demo apps" ON)

add_custom_target(pull_apps)

macro(sdk_add_subdirectory NAME BRANCH)
	if(IS_PULL OR NOT EXISTS ${BASE_DIRECTORY}/${NAME})
		cmsdk_git_clone_or_pull_branch(${BASE_DIRECTORY} ${NAME} ssh://git@github.com/StratifyLabs/${NAME}.git ${BRANCH})
	endif()
	add_subdirectory(${BASE_DIRECTORY}/${NAME} ${NAME})
	add_custom_target(pull_${NAME}
		COMMAND git pull
		WORKING_DIRECTORY ${BASE_DIRECTORY}/${NAME}
		)
	add_dependencies(pull_apps pull_${NAME})
endmacro()

if(APPS_IS_DEMO)
	sdk_add_subdirectory(HelloWorld main)
	sdk_add_subdirectory(Blinky main)
endif()

if(APPS_IS_TESTSUITE)
	#sdk_add_subdirectory(testsuite main)
endif()



