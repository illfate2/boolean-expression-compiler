cmake_minimum_required(VERSION 3.17)
project(1lab)

set(CMAKE_CXX_STANDARD 14)

add_executable(1lab
        main.cpp terminal.h non_terminal.h parser.h expression.h lexer.h)
