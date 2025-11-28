cmake_minimum_required(VERSION 3.5)
project(DUMA_CONF)

# See README.md for help building and using DUMA

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED off)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(createconf ../createconf.c)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
