cmake_minimum_required(VERSION 3.9)
project (zad2 C)

include_directories("${PROJECT_SOURCE_DIR}/../zad1")

add_executable(zad2 test.c)
add_library(zad2_static STATIC ../zad1/library.c ../zad1/library.h)
add_library(zad2_shared SHARED ../zad1/library.c ../zad1/library.h)

target_link_libraries(zad2 zad2_static)


