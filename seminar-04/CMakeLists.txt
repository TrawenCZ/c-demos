cmake_minimum_required(VERSION 3.0)
project(seminar04)

set(MATRIX_SOURCE matrix.c)
set(TICTAC_SOURCE tictac.c)

add_executable(matrix ${MATRIX_SOURCE})
add_executable(tictactoe ${TICTAC_SOURCE})
# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb3 -Wall -Wextra -pedantic")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(matrix PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(tictactoe PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(tictactoe PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(matrix PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()



