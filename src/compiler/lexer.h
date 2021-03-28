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

const std::unordered_set<char> symbols = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

bool isSymbol(char ch) { return symbols.find(ch) != symbols.end(); }

bool isConstant(char ch) {
    return ch == '1' | ch == '0';
}

enum class TokenType {
    OPEN_BRACKET,
    CLOSE_BRACKET,
    EQUALITY,
    IMPLICATION,
    NOT_OPERATOR,
    AND_OPERATOR,
    OR_OPERATOR,
    SYMBOL,
    CONSTANT,
};

std::ostream &operator<<(std::ostream &os, const TokenType &tokenType) {
    switch (tokenType) {
        case TokenType::OPEN_BRACKET:
            os << "open bracket";
            return os;
        case TokenType::CLOSE_BRACKET:
            os << "close bracket";
            return os;
        case TokenType::AND_OPERATOR:
            os << "and operator";
            return os;
        case TokenType::OR_OPERATOR:
            os << "or operator";
            return os;
        case TokenType::SYMBOL:
            os << "type";
            return os;
        case TokenType::CONSTANT:
            os << "constant";
            return os;
        case TokenType::NOT_OPERATOR:
            os << "not operator";
            return os;
    }
    return os;
}

struct Token {
public:
    TokenType type;
    size_t id;
    int64_t position;
    std::string value;
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "token type: " << token.type << ", at position: " << token.position << ", with value: " << token.value;
    return os;
}


class Lexer {
public:
    explicit Lexer(const std::string &str) {
        symbol_stream = std::make_unique<std::istringstream>(std::istringstream(str));
    }

    explicit Lexer(std::unique_ptr<std::istream> &&ss) : symbol_stream(std::move(ss)) {
    }

    Token GetNext() {
        auto token = getNext();
        token_table[token.id] = token;
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

    const std::unordered_map<size_t, Token> &GetTable() const {
        return token_table;
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
            case '&':
                return TokenType::AND_OPERATOR;
            case '|':
                return TokenType::OR_OPERATOR;
            default:
                if (isConstant(symbol)) {
                    return TokenType::CONSTANT;
                } else if (isSymbol(symbol)) {
                    return TokenType::SYMBOL;
                }
        }
        return {};
    }

    size_t id_counter = 0;
    std::unique_ptr<std::istream> symbol_stream;
    std::unordered_map<size_t, Token> token_table;
};

#endif //INC_1LAB_LEXER_H
