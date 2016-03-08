project(lesson01)
cmake_minimum_required(VERSION 2.8)

include_directories(
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}/source
	${PROJECT_SOURCE_DIR}/include
)

set(APPLICATION_NAME ${PROJECT_NAME})
set(APPLICATION_VERSION 0)

set(FRAMEWORK_NAME "vklessons")
set(FRAMEWORK_VERSION 0)

configure_file(
	"${PROJECT_SOURCE_DIR}/config.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

file(GLOB VK_SOURCES
	source/render/*.cpp
	source/render/*.hpp
	source/main.cpp
)

add_executable(${PROJECT_NAME} ${VK_SOURCES})
