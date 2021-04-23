cmake_minimum_required(VERSION 3.0)
project(seminar08)

# libraries
add_library(readline STATIC readline.c)
add_library(list STATIC list.c list_utils.c)

# 01 censor
add_executable(01-censor censor.c)
target_link_libraries(01-censor PRIVATE readline)

# 02 xor cipher
add_executable(02-cipher cipher.c)

# 03 filesort
add_executable(03-filesort filesort.c)
target_link_libraries(03-filesort PRIVATE readline list)

# 04 packets
add_executable(04-packets packets.c)

# 05 getline
add_executable(05-getline getline.c playground.c)

# copy example files
file(GLOB DATAFILES "data/*")
foreach(FILE ${DATAFILES})
    configure_file(${FILE} "." COPYONLY)
endforeach()

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -ggdb3 -Wall -Wextra -pedantic")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(01-censor     PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(02-cipher     PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(03-filesort   PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(04-packets    PRIVATE _CRT_SECURE_NO_DEPRECATE)
  target_compile_definitions(05-getline    PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(01-censor     PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(02-cipher     PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(03-filesort   PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(04-packets    PRIVATE __USE_MINGW_ANSI_STDIO=1)
    target_compile_definitions(05-getline    PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()
