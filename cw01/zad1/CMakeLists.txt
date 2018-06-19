cmake_minimum_required(VERSION 3.9)
project(zad1 C)

add_executable(zad1 test.c)
add_library(zad1_static STATIC library.c library.h)
add_library(zad1_shared SHARED library.c library.h)

target_link_libraries(zad1 zad1_static)

set(CMAKE_C_FLAGS "-Os -pedantic -Wextra -Wall")



