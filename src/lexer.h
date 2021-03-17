//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_LEXER_H
#define INC_1LAB_LEXER_H

#include <string>
#include <utility>
#include <unordered_map>
#include <sstream>
#include <cassert>

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
}

struct Token {
public:
    TokenType type;
    size_t id;
    size_t position;
    std::string value;
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "token type: " << token.type << ", at position: " << token.position << ", with value: " << token.value;
    return os;
}

class Lexer {
public:
    explicit Lexer(const std::string &str) {
        symbol_stream = std::stringstream(str);
    }

    Token GetNext() {
        auto[type, value] = getNext();
        Token token{.type=type, .id=id_counter++, .position=static_cast<size_t>(symbol_stream.tellg()), .value=value};
        token_table[token.id] = token;
        return token;
    }

    Token LookupNext() {
        auto[type, value] = lookupNext();
        size_t pos = symbol_stream.tellg();
        Token token{.type=type, .position=pos, .value=value};
        return token;
    }

    bool IsEmpty() const {
        return symbol_stream.eof();
    }

    const std::unordered_map<size_t, Token> &GetTable() const {
        return token_table;
    }


private:

    std::pair<TokenType, std::string> getNext() {
        if (symbol_stream.eof()){
            throw std::runtime_error("eof");
        }
        char token = symbol_stream.get();

        TokenType type;
        char next = symbol_stream.peek();

        switch (token) {
            case '(':
                type = TokenType::OPEN_BRACKET;
                break;
            case ')':
                type = TokenType::CLOSE_BRACKET;
                break;
            case '!':
                type = TokenType::NOT_OPERATOR;
                break;
            case '&':
                type = TokenType::AND_OPERATOR;
                break;
            case '|':
                type = TokenType::OR_OPERATOR;
                break;
            case ' ':
                return getNext();
            case '\\':
                assert(next == '/');
                symbol_stream.ignore();
                type = TokenType::OR_OPERATOR;
                break;
            case '/':
                assert(next == '\\');
                symbol_stream.ignore();
                type = TokenType::AND_OPERATOR;
                break;
            default:
                if (isConstant(token)) {
                    type = TokenType::CONSTANT;
                    break;
                }

                if (isSymbol(token)) {
                    type = TokenType::SYMBOL;
                    break;
                }
                if (token != '\0' && token != -1) {
                    std::stringstream ss;
                    ss << "unsupported type: " << token << " at position " << std::to_string(symbol_stream.tellg());
                    throw std::invalid_argument(ss.str());
                }
        }
        return {type, {token}};
    }

    std::pair<TokenType, std::string> lookupNext() {
        char token = symbol_stream.peek();

        size_t pos = symbol_stream.tellg();
        symbol_stream.seekg(pos + 1);
        TokenType type;
        char next = symbol_stream.peek();
        symbol_stream.seekg(pos);

        switch (token) {
            case '(':
                type = TokenType::OPEN_BRACKET;
                break;
            case ')':
                type = TokenType::CLOSE_BRACKET;
                break;
            case '!':
                type = TokenType::NOT_OPERATOR;
                break;
            case '&':
                type = TokenType::AND_OPERATOR;
                break;
            case '|':
                type = TokenType::OR_OPERATOR;
                break;
            case '\\':
                assert(next == '/');
                type = TokenType::OR_OPERATOR;
                break;
            case '/':
                assert(next == '\\');
                type = TokenType::AND_OPERATOR;
                break;
            default:
                if (isConstant(token)) {
                    type = TokenType::CONSTANT;
                    break;
                }

                if (token == ' ') {
                    symbol_stream.ignore();
                    return lookupNext();
                }
                if (isSymbol(token)) {
                    type = TokenType::SYMBOL;
                    break;
                }
                if (token != '\0' && token != -1) {
                    std::string err_msg = "unsupported type: ";
                    err_msg += token;
                    err_msg += " at position ";
                    err_msg += std::to_string(symbol_stream.tellg());
                    throw std::invalid_argument(err_msg);
                }
        }
        return {type, {token}};
    }

    size_t id_counter = 0;
    std::stringstream symbol_stream;
    std::unordered_map<size_t, Token> token_table;
};

#endif //INC_1LAB_LEXER_H
