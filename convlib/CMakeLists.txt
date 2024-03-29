# convutils/convlib
cmake_minimum_required(VERSION 3.20)

# Get library headers & source files
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
file(GLOB SRCS 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

# Create static library
add_library(convlib STATIC "${SRCS}")

# Set library properties
set_property(TARGET convlib PROPERTY CXX_STANDARD 20)
set_property(TARGET convlib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET convlib PROPERTY POSITION_INDEPENDENT_CODE ON)
if (MSVC)
	target_compile_options(convlib PUBLIC "/Zc:__cplusplus" "/Zc:preprocessor")
endif()

# Include library headers
include(PrependEach)
PREPEND_EACH(HEADERS_ABS "${HEADERS}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(convlib PUBLIC 
	"$<BUILD_INTERFACE:${HEADERS_ABS}>"
	"$<INSTALL_INTERFACE:${HEADERS}>"
)
# Set library include dirs
target_include_directories(convlib PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

# Link library dependencies
target_link_libraries(convlib PUBLIC shared tokenlib TermAPI strlib filelib optlib)

