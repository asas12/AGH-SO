cmake_minimum_required(VERSION 2.8)
project(zad2 C)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

add_executable(barber barber.c)
target_link_libraries(barber pthread)
target_link_libraries(barber rt)
add_executable(client client.c)
target_link_libraries(client pthread)
target_link_libraries(client rt)
