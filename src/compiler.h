//
// Created by illfate on 3/16/21.
//

#ifndef INC_1LAB_COMPILER_H
#define INC_1LAB_COMPILER_H

#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <exception>
#include <optional>
#include <utility>


class Compiler {
public:
    explicit Compiler(std::string str) : source(std::move(str)) {}

    std::optional<std::string> IsPDNF() {
        try {
            Lexer lexer(source);
            auto parser = Parser(lexer);
            parser.build();
            SemanticAnalyzer analyzer(parser.GetRoot());
            return analyzer.Is();
        } catch (const std::exception &ex) {
            return {ex.what()};
        }
    }

private:
    std::string source;
};

#endif //INC_1LAB_COMPILER_H
