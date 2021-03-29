#include "cli_runner.h"

// todo ivash logic
int main(int argc, char *argv[]) {
    CLIRunner runner(argc, argv);
    runner.Run(std::cout);
}