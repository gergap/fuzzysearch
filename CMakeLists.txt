project(fuzzysearch C)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -fno-omit-frame-pointer -Wall -Wextra")

find_package(Curses)
if (CURSES_FOUND)
    add_definitions(-DHAVE_CURSES)
endif()

add_definitions(-D_DEBUG)

add_executable(fuzzysearch main.c tagfile.c list.c metric.c)
if (CURSES_FOUND)
    target_link_libraries(fuzzysearch ${CURSES_LIBRARY})
endif()

