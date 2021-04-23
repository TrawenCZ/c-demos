cmake_minimum_required(VERSION 3.0)
project(seminar06)

set(SOURCE_FILES
        dynamicLib.c)


add_executable(playground06 playground.c ${SOURCE_FILES})
add_executable(tests06 unit_tests.c ${SOURCE_FILES})

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb3 -Wall -Wextra -pedantic")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(playground06 PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(tests06 PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(playground06 PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(tests06 PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()



