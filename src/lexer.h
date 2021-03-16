//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_LEXER_H
#define INC_1LAB_LEXER_H

#include <string>
#include <utility>
#include <unordered_map>
#include <sstream>

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

struct Token {
public:
    TokenType token;
    size_t id;
    size_t position;
    std::string value;

    Token() {

    }
};

class Lexer {
public:
    Lexer(std::string str) : str(std::move(str)) {
//        symbol_stream=std::stringstream(str);
    }

    Token GetNext() {
        TokenType type = getNext();
        Token token;
        token.token = type;
        token.id = id_counter++;
        token.position = counter - 1;
        token.value = {GetPrev()};
        token_table[token.id] = token;
        return token;
    }

    bool IsEmpty() const {
        return str.size() == (counter - 1);
    }

    const std::unordered_map<size_t, Token> &GetTable() const {
        return token_table;
    }

    Token LookupNext() {
        Token next = GetNext();
        --counter;
        return next;
    }

private:

    char GetPrev() {
        return str[counter - 1];
    }

    TokenType getNext() {
        char token = str[counter++];
        TokenType type;
        char next = str[counter];

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
                type = getNext();
                break;
            case '\\':
                assert(next == '/');
                counter++;
                type = TokenType::OR_OPERATOR;
                break;
            case '/':
                assert(next == '\\');
                counter++;
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
                if (token != '\0') {
                    std::string err_msg = "unsupported symbol: ";
                    err_msg += token;
                    err_msg += " at position ";
                    err_msg += std::to_string(counter);
                    throw std::invalid_argument(err_msg);
                }
        }
        return type;
    }

    char LookupNextSymbol() {
        return str[counter + 1];
    }

    std::string str;
    size_t id_counter = 0;
    size_t counter = 0;
    std::stringstream symbol_stream;
    std::unordered_map<size_t, Token> token_table;
};

#endif //INC_1LAB_LEXER_H
