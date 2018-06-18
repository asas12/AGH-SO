#!/bin/bash

./zad1 generate dane4 1000000 4
./zad1 generate dane512 100000 512
./zad1 generate dane4096 50000 4096
./zad1 generate dane8192 50000 8192

#testing copying
./zad1 copy dane4 dane4s 100000 4 sys >> wyniki.txt
./zad1 copy dane512 dane512s 50000 512 sys >> wyniki.txt
./zad1 copy dane4096 dane4096s 50000 4096 sys >> wyniki.txt
./zad1 copy dane8192 dane8192s 50000 8192 sys >> wyniki.txt

./zad1 copy dane4 dane4s2 100000 4 sys >> wyniki.txt
./zad1 copy dane512 dane512s2 50000 512 sys >> wyniki.txt
./zad1 copy dane4096 dane4096s2 50000 4096 sys >> wyniki.txt
./zad1 copy dane8192 dane8192s2 50000 8192 sys >> wyniki.txt

./zad1 copy dane4 dane4l 1000000 4 lib >> wyniki.txt
./zad1 copy dane512 dane512l 100000 512 lib >> wyniki.txt
./zad1 copy dane4096 dane4096l 50000 4096 lib >> wyniki.txt
./zad1 copy dane8192 dane8192l 50000 8192 lib >> wyniki.txt

./zad1 copy dane4 dane4l2 1000000 4 lib >> wyniki.txt
./zad1 copy dane512 dane512l2 100000 512 lib >> wyniki.txt
./zad1 copy dane4096 dane4096l2 50000 4096 lib >> wyniki.txt
./zad1 copy dane8192 dane8192l2 50000 8192 lib >> wyniki.txt

#testing sorting
./zad1 sort dane4s 2000 4 sys >> wyniki.txt
./zad1 sort dane512s 500 512 sys >> wyniki.txt
./zad1 sort dane4096s 500 4096 sys >> wyniki.txt
./zad1 sort dane8192s 500 8192 sys >> wyniki.txt

./zad1 sort dane4s2 2000 4 sys >> wyniki.txt
./zad1 sort dane512s2 500 512 sys >> wyniki.txt
./zad1 sort dane4096s2 500 4096 sys >> wyniki.txt
./zad1 sort dane8192s2 500 8192 sys >> wyniki.txt

./zad1 sort dane4l 2000 4 lib >> wyniki.txt
./zad1 sort dane512l 1000 512 lib >> wyniki.txt
./zad1 sort dane4096l 500 4096 lib >> wyniki.txt
./zad1 sort dane8192l 500 8192 lib >> wyniki.txt

./zad1 sort dane4l2 2000 4 lib >> wyniki.txt
./zad1 sort dane512l2 500 512 lib >> wyniki.txt
./zad1 sort dane4096l2 100 4096 lib >> wyniki.txt
./zad1 sort dane8192l2 100 8192 lib >> wyniki.txt
