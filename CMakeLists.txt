cmake_minimum_required(VERSION 3.25)
project(UniPix C)

set(CMAKE_C_STANDARD 11)

add_executable(UniPix main.c UniPix.h Matrix.h format/PPM.h Math.h)
