#include <memory>
#include <set>
#include <stack>
#include <unordered_set>
#include <vector>
#include "compiler/parser.h"
#include <ranges>
#include "../vendor/argparse.hpp"
#include "cli_runner.h"

int main(int argc, char *argv[]) {
    CLIRunner runner(argc, argv);
    runner.Run(std::cout);

}