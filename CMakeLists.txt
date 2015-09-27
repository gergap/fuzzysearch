project(fuzzysearch C)
cmake_minimum_required(VERSION 2.8)

option(BUILD_TESTING "Enable Unittests" OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -fno-omit-frame-pointer -Wall -Wextra")

find_package(Curses)
if (CURSES_FOUND)
    add_definitions(-DHAVE_CURSES)
endif()

#add_definitions(-D_DEBUG)

add_executable(fuzzysearch main.c tagfile.c list.c metric.c)
if (CURSES_FOUND)
    target_link_libraries(fuzzysearch ${CURSES_LIBRARY})
endif()
install(TARGETS fuzzysearch DESTINATION bin)

enable_testing()

if (BUILD_TESTING)
    add_subdirectory(testlib)
    add_subdirectory(test)
endif()
