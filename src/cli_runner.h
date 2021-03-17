//
// Created by illfate on 3/17/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H
#define BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H

#include "tests/tests.h"
#include <fstream>
#include <memory>

class CLIRunner {
public:
    CLIRunner(int argc, char *argv[]) : argc(argc), argv(argv) {
        cli_parser = argparse::ArgumentParser("boolean-expression-compiler");
        cli_parser.add_argument(test_flag)
                .help("run tests")
                .default_value(false)
                .implicit_value(true);

        cli_parser.add_argument(file_flag)
                .help("specify file with a formula");

        cli_parser.add_argument(formula_flag)
                .help("specify a formula string");
    }

    void Run(std::ostream &os) {
        init();
        if (is_test) {
            RunTests();
        } else if (formula) {
            processCompilerIsPDNF(std::make_unique<std::stringstream>(formula.value()));
        } else if (file_name) {
            auto fstream_ptr = std::make_unique<std::ifstream>(std::ifstream());
            fstream_ptr->open(file_name.value());
            processCompilerIsPDNF(std::make_unique<std::stringstream>(formula.value()));
        } else {
            std::cout << "no arguments were specified\n";
        }

    }

private:
    void processCompilerIsPDNF(std::unique_ptr<std::istream> &&is) {
        Compiler compiler(std::move(is));
        auto pdnf_err = compiler.IsPDNF();
        if (pdnf_err) {
            std::cout << "false: " << pdnf_err << "\n";
        } else {
            std::cout << "true\n";
        }
    }

    void init() {
        cli_parser.parse_args(argc, argv);
        is_test = cli_parser[test_flag] == true;
        try {
            std::string parser_file_name = cli_parser.get(file_flag);
            file_name = parser_file_name;
        }
        catch (const std::exception &exception) {}
        try {
            std::string parser_formula = cli_parser.get(formula_flag);
            file_name = parser_formula;
        }
        catch (const std::exception &exception) {}

    }

    argparse::ArgumentParser cli_parser;
    bool is_test;
    std::optional<std::string> file_name;
    std::optional<std::string> formula;
    const std::string test_flag = "--test";
    const std::string file_flag = "--file";
    const std::string formula_flag = "--formula";
    int argc;
    char **argv;
};


#endif //BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H
