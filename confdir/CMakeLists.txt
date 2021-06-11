cmake_minimum_required(VERSION 3.1)
project(DUMA_CONF)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED off)

add_executable(createconf ../createconf.c)
