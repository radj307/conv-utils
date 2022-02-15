# conv-utils/conv2
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

MAKE_VERSION_HEADER(
	"${CMAKE_CURRENT_SOURCE_DIR}/version.h"
	"CONV2"
	"${CONV_VERSION}"
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
	target_compile_options(conv2 PUBLIC "/Zc:__cplusplus")
endif()

include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}/")
target_sources(conv2 PUBLIC "$<BUILD_INTERFACE:${HEADERS_ABS}>" "$<INSTALL_INTERFACE:${HEADERS}>")

target_link_libraries(conv2 PUBLIC shared strlib TermAPI optlib convlib)

include(PackageInstaller)
INSTALL_EXECUTABLE(conv2 "${CMAKE_INSTALL_PREFIX}/bin")
