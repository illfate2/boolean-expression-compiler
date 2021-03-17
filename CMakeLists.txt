cmake_minimum_required(VERSION 3.16)
project(boolean-expression-compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        boolean-expression-compiler
        vendor/argparse.hpp
        src/main.cpp src/compiler/terminal.h src/compiler/non_terminal.h src/compiler/parser.h src/compiler/expression.h src/compiler/lexer.h src/compiler/semantic_analyzer.h src/compiler/compiler.h src/tests/tests.h src/cli_runner.h)
