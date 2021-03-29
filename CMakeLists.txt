cmake_minimum_required(VERSION 3.16)
project(boolean-expression-compiler)

Include(FetchContent)
set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.1)

FetchContent_MakeAvailable(Catch2)

add_executable(
        boolean-expression-compiler
        vendor/argparse.hpp
        src/main.cpp src/compiler/terminal.h src/compiler/non_terminal.h src/compiler/parser.h src/compiler/expression.h src/compiler/lexer.h src/compiler/semantic_analyzer.h src/compiler/compiler.h src/cli_runner.h src/compiler/symbol_table.h src/compiler/token.h vendor/text_table.h)
add_executable(tests src/tests/tests.cpp)

enable_testing()
add_test(NAME tests COMMAND test)
target_link_libraries(tests PRIVATE Catch2::Catch2)
