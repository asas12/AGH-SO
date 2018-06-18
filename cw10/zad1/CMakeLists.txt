cmake_minimum_required(VERSION 2.8)
project(zad1 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")


add_executable(server server.c)
target_link_libraries(server m)
target_link_libraries(server pthread)

add_executable(client client.c)
target_link_libraries(client m)
target_link_libraries(client pthread)

