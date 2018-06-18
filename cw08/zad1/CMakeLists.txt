cmake_minimum_required(VERSION 2.8)
project(zad1 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(zad1 zad1.c)
target_link_libraries(zad1 m)
target_link_libraries(zad1 pthread)
add_executable(filter filter.c)


