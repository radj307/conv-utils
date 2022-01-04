# convutils/convlib
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Fix MSVC __cplusplus macro
endif()

project(convlib VERSION ${CONV_VERSION} LANGUAGES CXX)

file(GLOB SRCS 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
	"src/*.c*"
)

add_library(convlib STATIC ${SRCS})

target_include_directories(convlib PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

target_link_libraries(convlib PUBLIC shared strlib optlib)

include(PackageInstaller)
#CREATE_PACKAGE(convlib SameMajorVersion)
#INSTALL_PACKAGE(convlib)
