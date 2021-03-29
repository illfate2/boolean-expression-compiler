//
// Created by illfate on 3/28/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_SYMBOL_TABLE_H
#define BOOLEAN_EXPRESSION_COMPILER_SYMBOL_TABLE_H

#include <unordered_map>
#include <optional>
#include "token.h"
#include "non_terminal.h"
#include "terminal.h"

class SymbolTable {
public:

    void SetToken(size_t id, const Token &token) {
        token_table[id] = token;
    }

    std::optional<Token> GetToken(size_t id) const {
        return token_table.at(id);
    }

    void SetTokenToConst(const Token &token, const Constant &con) {
        token_to_constant.insert({token, con});
    }

    const std::unordered_map<Token, Constant> &getTokenToConstant() const {
        return token_to_constant;
    }

private:
    std::unordered_map<size_t, Token> token_table;
    std::unordered_map<Token, Constant> token_to_constant;
};

#endif //BOOLEAN_EXPRESSION_COMPILER_SYMBOL_TABLE_H
