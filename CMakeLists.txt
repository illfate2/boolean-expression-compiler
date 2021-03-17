cmake_minimum_required(VERSION 3.17)
project(1lab)

set(CMAKE_CXX_STANDARD 20)

add_executable(1lab
        main.cpp src/terminal.h src/non_terminal.h src/parser.h src/expression.h src/lexer.h src/semantic_analyzer.h src/compiler.h)
