cmake_minimum_required(VERSION 3.0)

# Project configuration
project(hw03)
set(SOURCES main.c scheduler.h scheduler.c)
set(EXECUTABLE queuectl)

# Executable
add_executable(queuectl ${SOURCES})

# Create option to enable/disable verbose output
# To disable debug output run
# cmake -DENABLE_DEBUG=OFF ${PATH_TO_PROJECT}
option(ENABLE_DEBUG "Enable verbose output of queuectl (default on)" ON)

if (ENABLE_DEBUG STREQUAL ON)
    # Enable debug output
    target_compile_definitions(queuectl PUBLIC CONFIG_ENABLE_DEBUG)
endif()

# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  # Strongly suggested: enable -Werror
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic -g -ggdb3")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(${EXECUTABLE} PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()
