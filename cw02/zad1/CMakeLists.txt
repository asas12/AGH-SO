cmake_minimum_required(VERSION 2.8)
project(zad1 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -D_BSD_SOURCE")

add_executable(zad1 zad1.c)
