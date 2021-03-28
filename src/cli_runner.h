//
// Created by illfate on 3/17/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H
#define BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H

#include <fstream>
#include <memory>
#include "../vendor/argparse.hpp"
#include "compiler/compiler.h"

class CLIRunner {
public:
    CLIRunner(int argc, char *argv[]) : argc(argc), argv(argv) {
        cli_parser = argparse::ArgumentParser("boolean-expression-compiler");

        cli_parser.add_argument(file_arg)
                .help("specify file with a formula");
        cli_parser.add_argument(is_pdnf_flag).default_value(false)
                .help("specify checking for pdnf flag").implicit_value(true);

        cli_parser.add_argument(formula_arg)
                .help("specify a formula string");
    }

    void Run(std::ostream &os) {
        init();
        if (is_pdnf) {
            if (formula) {
                processCompilerIsPDNF(std::make_unique<std::stringstream>(formula.value()));
            } else if (file_name) {
                auto fstream = std::ifstream();
                fstream.open(file_name.value());
                processCompilerIsPDNF(std::make_unique<std::ifstream>(std::move(fstream)));
            }
        } else {
            std::cout << "no arguments were specified\n";
        }

    }

private:
    void processCompilerIsPDNF(std::unique_ptr<std::istream> &&is) {
        Compiler compiler(std::move(is));
        auto pdnf_err = compiler.IsPDNF();
        if (pdnf_err) {
            std::cout << "This formula isn't in PDNF: " << pdnf_err.value() << "\n";
        } else {
            std::cout << "This formula is in PDNF.\n";
        }
    }

    void init() {
        cli_parser.parse_args(argc, argv);
        try {
            std::string parser_file_name = cli_parser.get(file_arg);
            file_name = parser_file_name;
        }
        catch (const std::exception &exception) {}

        try {
            std::string parser_formula = cli_parser.get(formula_arg);
            formula = parser_formula;
        }
        catch (const std::exception &exception) {}

        bool is_pdnf_value = cli_parser[is_pdnf_flag] == true;
        is_pdnf = is_pdnf_value;
    }

    argparse::ArgumentParser cli_parser;
    std::optional<std::string> file_name;
    std::optional<std::string> formula;
    std::optional<bool> is_pdnf;
    const std::string file_arg = "--file";
    const std::string formula_arg = "--formula";
    const std::string is_pdnf_flag = "--pdnf";
    int argc;
    char **argv;
};


#endif //BOOLEAN_EXPRESSION_COMPILER_CLI_RUNNER_H
