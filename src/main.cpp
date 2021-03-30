#include "cli_runner.h"

int main(int argc, char *argv[]) {
    CLIRunner runner(argc, argv);
    runner.Run(std::cout);
}