project(fuzzysearch C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -fno-omit-frame-pointer")

add_definitions(-D_DEBUG)

add_executable(fuzzysearch main.c)


