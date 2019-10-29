cmake_minimum_required(VERSION 3.14)
project(grammar C)

set(CMAKE_C_STANDARD 99)

add_executable(grammar grammar.c grammar.h word.c word.h error.h)