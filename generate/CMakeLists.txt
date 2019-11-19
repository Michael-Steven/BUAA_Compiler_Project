cmake_minimum_required(VERSION 3.15)
project(generate C)

set(CMAKE_C_STANDARD 99)

add_executable(generate parser.c parser.h word.c word.h error.h error.c file.h main.c middle_code.h middle_code.c mips.h mips.c)