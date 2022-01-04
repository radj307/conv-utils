# convutils/conv
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Fix MSVC __cplusplus macro
endif()

project(conv VERSION ${CONV_VERSION} LANGUAGES CXX)

file(GLOB SRCS 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.h*"
	"*.c*"
)

add_executable(conv "main.cpp" ${SRCS})

target_link_libraries(conv PUBLIC optlib TermAPI convlib)
