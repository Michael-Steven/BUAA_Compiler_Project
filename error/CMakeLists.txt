cmake_minimum_required(VERSION 3.15)
project(error C)

set(CMAKE_C_STANDARD 99)

add_executable(error grammar.c grammar.h word.c word.h error.h)