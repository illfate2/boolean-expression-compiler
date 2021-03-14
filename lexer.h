//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_LEXER_H
#define INC_1LAB_LEXER_H

#include <string>
#include <utility>

const std::unordered_set<char> symbols = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

bool isSymbol(char ch) { return symbols.find(ch) != symbols.end(); }

enum class Token {
    OPEN_BRACKET,
    CLOSE_BRACKET,
    NOT_OPERATOR,
    AND_OPERATOR,
    OR_OPERATOR,
    SYMBOL,
};

class Lexer {
public:
    Lexer(std::string str) : str(std::move(str)) {}

    Token GetNext() {
        char token = str[counter++];
        switch (token) {
            case '(':
                return Token::OPEN_BRACKET;
            case ')':
                return Token::CLOSE_BRACKET;
            case '!':
                return Token::NOT_OPERATOR;
            case '&':
                return Token::AND_OPERATOR;
            case '|':
                return Token::OR_OPERATOR;
            default:
                if (isSymbol(token)) {
                    return Token::SYMBOL;
                }
                if (token != '\0') {
                    std::string err_msg = "unsupported symbol: ";
                    err_msg += token;
                    err_msg+=" at position ";
                    err_msg+=std::to_string(counter);
                    throw std::invalid_argument(err_msg);
                }
        }
    }

    char GetCurrent() {
        return str[counter];
    }

    char GetPrev() {
        return str[counter - 1];
    }

//    Token LookupNext(){
//        return str[counter+1];
//    }
private:
    std::string str;
    size_t counter = 0;
};

#endif //INC_1LAB_LEXER_H
