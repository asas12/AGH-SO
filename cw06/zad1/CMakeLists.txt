cmake_minimum_required(VERSION 2.8)
project(zad1 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(server server.c)
#add_library(ipcutil.h ipcutil.h)
add_executable(client client.c)
