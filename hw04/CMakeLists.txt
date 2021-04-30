cmake_minimum_required(VERSION 3.0)

project(merger)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-std=c99 -pedantic -Wall -Wextra")

add_subdirectory(src)
add_subdirectory(test)
