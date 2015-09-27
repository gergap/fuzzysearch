project(fuzzysearch C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -fno-omit-frame-pointer -Wall -Wextra")

find_package(Curses)

add_definitions(-D_DEBUG)

add_executable(fuzzysearch main.c tagfile.c list.c metric.c)
target_link_libraries(fuzzysearch ${CURSES_LIBRARY})

