# convutils/
cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307lib/307modules")

set(ENV{CONV2_VERSION} "0.0.0")
include(VersionTag)
GET_VERSION_TAG("${CMAKE_CURRENT_SOURCE_DIR}" "CONV2")

project(convutils VERSION "${CONV2_VERSION}" LANGUAGES CXX)

add_subdirectory("307lib")

add_subdirectory("convlib")

add_subdirectory("conv2")
