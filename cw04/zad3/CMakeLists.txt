cmake_minimum_required(VERSION 2.8)
project(zad3)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")


add_executable(zad3.c)
#target_link_libraries(zad2 pthread) #for semaphores
#target_link_libraries(zad2 rt) #for mmap
