cmake_minimum_required(VERSION 3.0.0)
project(coroutines_test VERSION 0.1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_package(asio REQUIRED)

add_executable(coroutines_test main.cpp)

target_include_directories(coroutines_test PUBLIC ${asio_INCLUDE})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
