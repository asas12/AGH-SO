cmake_minimum_required(VERSION 2.8)
project(zad2 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(master master.c)
add_executable(slave slave.c)
