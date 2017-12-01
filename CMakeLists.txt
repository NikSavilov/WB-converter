cmake_minimum_required(VERSION 3.8)
project(BW)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c header.h source.c)
add_executable(BW ${SOURCE_FILES})