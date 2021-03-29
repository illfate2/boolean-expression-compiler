//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_LEXER_H
#define INC_1LAB_LEXER_H

#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <cassert>
#include <functional>
#include <memory>
#include "symbol_table.h"
#include "token.h"

class Lexer {
public:
    explicit Lexer(const std::string &str, const std::shared_ptr<SymbolTable> &symbol_table) : symbol_table(
            symbol_table) {
        symbol_stream = std::make_unique<std::istringstream>(std::istringstream(str));
    }

    explicit Lexer(std::unique_ptr<std::istream> &&ss, const std::shared_ptr<SymbolTable> &symbol_table)
            : symbol_stream(std::move(ss)), symbol_table(symbol_table) {
    }

    Token GetNext() {
        auto token = getNext();
        symbol_table->SetToken(token.id, token);
        return token;
    }

    Token LookupNext() {
        auto[type, value] = lookupNext();
        int64_t pos = symbol_stream->tellg();
        Token token{.type=type, .position=pos, .value=value};
        return token;
    }

    bool IsEmpty() const {
        return symbol_stream->eof();
    }

private:
    Token getNext() {
        if (symbol_stream->eof()) {
            throw std::runtime_error("unexpected eof");
        }
        char token = symbol_stream->get();
        int64_t pos = symbol_stream->tellg();

        TokenType type;
        char next = symbol_stream->peek();

        auto opt_type = getBaseTokenType(token, pos);
        if (!opt_type) {
            if (token == '-') {
                assert(next == '>');
                symbol_stream->ignore();
                type = TokenType::IMPLICATION;
            } else if (token == 'l') {
                assert(next == 'e');
                symbol_stream->ignore();
                token = symbol_stream->get();
                assert(token == 't');

                next = symbol_stream->peek();
                assert(next == ' ');
                type = TokenType::IDENTIFIER_OPERATOR;
            } else if (token == ' ' || token == '\n') {
                return getNext();
            } else if (token == '/') {
                assert(next == '\\');
                symbol_stream->ignore();
                type = TokenType::AND_OPERATOR;
            } else if (token == '\\') {
                assert(next == '/');
                symbol_stream->ignore();
                type = TokenType::OR_OPERATOR;
            } else if (token != '\0' && token != -1) {
                std::stringstream ss;
                ss << "unsupported type: " << token << " at position " << std::to_string(pos);
                throw std::invalid_argument(ss.str());
            }
        } else {
            type = opt_type.value();
        }
        Token res_token{.type=type, .id=id_counter++, .position=pos, .value={token}};
        return res_token;
    }

    std::pair<TokenType, std::string> lookupNext() {
        char token = symbol_stream->peek();

        size_t pos = symbol_stream->tellg();
        symbol_stream->seekg(pos + 1);
        TokenType type;
        char next = symbol_stream->peek();
        symbol_stream->seekg(pos);

        auto opt_type = getBaseTokenType(token, pos);
        if (!opt_type) {
            if (token == '-') {
                assert(next == '>');
                type = TokenType::IMPLICATION;
            } else if (token == ' ' || token == '\n') {
                symbol_stream->ignore();
                return lookupNext();
            } else if (token == '/') {
                assert(next == '\\');
                type = TokenType::AND_OPERATOR;
            } else if (token == '\\') {
                assert(next == '/');
                type = TokenType::OR_OPERATOR;
            } else if (token != '\0' && token != -1) {
                std::string err_msg = "unsupported type: ";
                err_msg += token;
                err_msg += " at position ";
                err_msg += std::to_string(pos);
                throw std::invalid_argument(err_msg);
            }
        } else {
            type = opt_type.value();
        }
        return {type, {token}};
    }

    std::optional<TokenType> getBaseTokenType(char symbol, int64_t pos) {
        switch (symbol) {
            case '(':
                return TokenType::OPEN_BRACKET;
            case ')':
                return TokenType::CLOSE_BRACKET;
            case '~':
                return TokenType::EQUALITY;
            case '!':
                return TokenType::NOT_OPERATOR;
            case '=':
                return TokenType::ASSIGNMENT_OPERATOR;
            case ';':
                return TokenType::CLOSE_EXPRESSION_OPERATOR;
//            case '&':
//                return TokenType::AND_OPERATOR;
//            case '|':
//                return TokenType::OR_OPERATOR;
            default:
                if (isConstant(symbol)) {
                    return TokenType::CONSTANT;
                } else if (isSymbol(symbol)) {
                    return TokenType::SYMBOL;
                }
        }

        return {};
    }

    size_t

            id_counter = 0;
    std::unique_ptr<std::istream> symbol_stream;
    std::shared_ptr<SymbolTable> symbol_table;
};

#endif //INC_1LAB_LEXER_H
