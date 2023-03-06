# conv-utils/conv2
cmake_minimum_required(VERSION 3.20)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rc")
MAKE_VERSION_HEADER(
	"${CMAKE_CURRENT_BINARY_DIR}/rc/version.h"
	"CONV2"
	"${conv2_VERSION_EXTENDED}"
)
include(CopyrightMaker)
MAKE_COPYRIGHT_NOTICE(conv2_COPYRIGHT "radj307" "2022")
if (WIN32)
	include(ResourceMaker)
	MAKE_STRINGRC_VERSIONINFO(
		_conv2_stringrc_versioninfo
		"${conv2_VERSION}"
		"${conv2_COPYRIGHT}"
		"radj307"
		"conv2"
		"Commandline value conversion utility."
	)
	MAKE_RESOURCE("${CMAKE_CURRENT_BINARY_DIR}/rc/conv2.rc" "${_conv2_stringrc_versioninfo}")
endif()

file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.h*"
)
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.c*"
)
file(GLOB RESOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_BINARY_DIR}/rc/*"
)

add_executable(conv2 "${SRCS}" "${RESOURCES}")

set_property(TARGET conv2 PROPERTY CXX_STANDARD 20)
set_property(TARGET conv2 PROPERTY CXX_STANDARD_REQUIRED ON)
if (MSVC)
	target_compile_options(conv2 PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

target_sources(conv2 PUBLIC "${HEADERS}")

target_include_directories(conv2 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/rc")

target_link_libraries(conv2 PUBLIC shared strlib TermAPI optlib convlib)

include(PackageInstaller)
INSTALL_EXECUTABLE(conv2 "${CMAKE_INSTALL_PREFIX}/bin")
