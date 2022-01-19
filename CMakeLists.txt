# convutils
cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/307lib/307modules")

include(AutoVersion)
GET_VERSION("${CMAKE_CURRENT_LIST_DIR}" CONV_VERSION_MAJOR CONV_VERSION_MINOR CONV_VERSION_PATCH)
MAKE_VERSION(CONV_VERSION "${CONV_VERSION_MAJOR}" "${CONV_VERSION_MINOR}" "${CONV_VERSION_PATCH}")

project(convutils VERSION ${CONV_VERSION} LANGUAGES CXX)

add_subdirectory("307lib")

add_subdirectory("convlib")

add_subdirectory("conv")
