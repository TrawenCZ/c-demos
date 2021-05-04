cmake_minimum_required(VERSION 3.0)
project(seminar10)

add_library(queue STATIC "queue.c")

add_executable(playground "playground.c")
target_link_libraries(playground queue)

add_executable(tests "tests.c")
target_link_libraries(tests queue)

add_executable(tests-debug "tests.c")
target_compile_definitions(tests-debug PRIVATE DEBUG)
target_link_libraries(tests-debug queue)

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb3 -Wall -Wextra -pedantic")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(tests PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(tests-debug PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(playground PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(tests PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(tests-debug PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(playground PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()


