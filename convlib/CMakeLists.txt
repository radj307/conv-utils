# convutils/convlib
cmake_minimum_required(VERSION 3.15)

if (MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	string(APPEND CMAKE_CXX_FLAGS " /Zc:__cplusplus") # Fix MSVC __cplusplus macro
endif()


project(convlib VERSION ${CONV_VERSION} LANGUAGES CXX)

file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
file(GLOB OBJS 
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
)

add_library(convlib STATIC ${OBJS})

include(SetProperties)
SET_PROPERTIES(convlib CXX_STANDARD 20)

include(PrependEach)
PREPEND_EACH(${SRCS} "${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(convlib PUBLIC 
	"$<BUILD_INTERFACE:${SRCS_ABS}>"
	"$<INSTALL_INTERFACE:${SRCS}>"
)

target_include_directories(convlib PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_SOURCE_DIR}/src>"
	"$<INSTALL_INTERFACE:include;src>"
)

target_link_libraries(convlib PUBLIC shared strlib optlib)

include(PackageInstaller)
#CREATE_PACKAGE(convlib SameMajorVersion)
#INSTALL_PACKAGE(convlib)
