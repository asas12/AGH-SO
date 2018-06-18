cmake_minimum_required(VERSION 3.9)
project (zad3a)

include_directories("${PROJECT_SOURCE_DIR}/../zad1")

add_executable(zad3static ../zad2/test.c)
add_executable(zad3shared ../zad2/test.c)
add_executable(zad3dynamic test.c)
add_library(zad3_static STATIC ../zad1/library.c ../zad1/library.h)
add_library(zad3_shared SHARED ../zad1/library.c ../zad1/library.h)


target_link_libraries(zad3static zad3_static)
target_link_libraries(zad3shared zad3_static)
target_link_libraries(zad3dynamic ${CMAKE_DL_LIBS}) 

add_custom_target(test1 ALL COMMAND ./zad3static cd 3000 1000 ar 1500 s 230 COMMAND echo Static library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt) 
add_custom_target(test2 ALL COMMAND ./zad3shared cd 3000 1000 ar 1500 s 230 COMMAND echo Shared library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt)
add_custom_target(test3 ALL COMMAND ./zad3dynamic cd 3000 1000 ar 1500 s 230 COMMAND echo Dynamic library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt)

add_custom_target(test1s ALL COMMAND ./zad3static cs ar 1500 s 230 COMMAND echo Static library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt) 
add_custom_target(test2s ALL COMMAND ./zad3shared cs ar 1500 s 230 COMMAND echo Shared library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt)
add_custom_target(test3s ALL COMMAND ./zad3dynamic cs ar 1500 s 230 COMMAND echo Dynamic library: >>results3a.txt COMMAND cat raport2.txt>>results3a.txt COMMAND rm ./raport2.txt)
