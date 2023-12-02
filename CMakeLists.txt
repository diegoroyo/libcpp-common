cmake_minimum_required(VERSION 3.10)

project(libcpp-common)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(geometry examples/geometry.cpp)