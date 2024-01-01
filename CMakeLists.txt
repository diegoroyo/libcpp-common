cmake_minimum_required(VERSION 3.10)

project(libcpp-common)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tests)

# main library
file(GLOB libcpp-common-SRC "src/*.cpp" "src/**/*.cpp")
add_library(libcpp-common STATIC ${libcpp-common-SRC})

# tests
add_executable(libcpp-common-run-tests tests/geometry/test_geometry.h tests/main.cpp include/libcpp-common/test.h)
target_link_libraries(libcpp-common-run-tests PRIVATE libcpp-common)

# examples
add_executable(geometry examples/geometry.cpp)
target_link_libraries(geometry PRIVATE libcpp-common)

add_executable(mesh examples/mesh.cpp)
target_link_libraries(mesh PRIVATE libcpp-common)

add_executable(bitmap examples/bitmap.cpp)
target_link_libraries(bitmap PRIVATE libcpp-common)
