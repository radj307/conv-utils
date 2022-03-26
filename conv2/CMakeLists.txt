# conv-utils/conv2
cmake_minimum_required(VERSION 3.20)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(NOT "${CONV2_VERSION_EXTRA0}" STREQUAL "")
	set(CONV2_VERSION_FULL "${CONV2_VERSION}-${CONV2_VERSION_EXTRA0}" CACHE INTERNAL "")
	message(STATUS "[STATUS]:  Version number contains more than 3 segments, the 4th segment will be included in (version.h)")
else()
	set(CONV2_VERSION_FULL "${CONV2_VERSION}" CACHE INTERNAL "")
	message(STATUS "[STATUS]:  Using version number \"${CONV2_VERSION_FULL}\"")
endif()

MAKE_VERSION_HEADER(
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h"
	"CONV2"
	"${CONV2_VERSION_FULL}"
)

file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.h*"
)

add_executable(conv2 "main.cpp")

set_property(TARGET conv2 PROPERTY CXX_STANDARD 20)
set_property(TARGET conv2 PROPERTY CXX_STANDARD_REQUIRED ON)
if (MSVC)
	target_compile_options(conv2 PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

target_sources(conv2 PUBLIC "${HEADERS}")

target_link_libraries(conv2 PUBLIC shared strlib TermAPI optlib convlib)

include(PackageInstaller)
INSTALL_EXECUTABLE(conv2 "${CMAKE_INSTALL_PREFIX}/bin")
