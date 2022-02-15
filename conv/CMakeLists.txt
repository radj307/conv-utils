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
)

include(ResourceMaker)
CREATE_VERSION_RESOURCE(
	"${CMAKE_CURRENT_BINARY_DIR}/conv.rc" 
	"${CONV_VERSION_MAJOR}" 
	"${CONV_VERSION_MINOR}"
	"${CONV_VERSION_PATCH}"
	"radj307"
	"Conversion & calculator utility"
	"conv.exe"
	"GPLv3"
	"conv.exe"
	"Conv-Utils"
)

add_executable(conv "main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/conv.rc")
set_property(TARGET conv PROPERTY CXX_STANDARD 20)
set_property(TARGET conv PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET conv PROPERTY POSITION_INDEPENDENT_CODE ON)
if (MSVC)
	target_compile_options(conv PUBLIC "/Zc:__cplusplus")
endif()

include(PrependEach)
PREPEND_EACH(SRCS_ABS ${SRCS} "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(conv PUBLIC
	"$<BUILD_INTERFACE:${SRCS_ABS}>"
	"$<INSTALL_INTERFACE:${SRCS}>"
)

target_link_libraries(conv PUBLIC TermAPI optlib filelib convlib)

include(PackageInstaller)
INSTALL_EXECUTABLE(conv "${CMAKE_INSTALL_PREFIX}/bin")
