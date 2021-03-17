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
#include <any>
#include <variant>


class Compiler {
public:
    explicit Compiler(std::string str) : source(std::move(str)) {}

    explicit Compiler(std::unique_ptr<std::istream> &&stream) : istream(std::move(stream)) {}


    std::optional<std::string> IsPDNF() {
        try {
            auto lexer = getLexer();
            auto parser = Parser(std::move(lexer));
            parser.build();
            SemanticAnalyzer analyzer(parser.GetRoot());
            return analyzer.IsPDNF();
        } catch (const std::exception &ex) {
            return {ex.what()};
        }
    }

private:

    std::unique_ptr<Lexer> getLexer() {
        if (istream) {
            return std::make_unique<Lexer>(Lexer(std::move(istream)));
        }
        return std::make_unique<Lexer>(Lexer(source));
    }

    std::string source;
    std::unique_ptr<std::istream> istream;
};

#endif //INC_1LAB_COMPILER_H
