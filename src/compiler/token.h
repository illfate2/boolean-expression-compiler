//
// Created by illfate on 3/28/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_TOKEN_H
#define BOOLEAN_EXPRESSION_COMPILER_TOKEN_H


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
    ASSIGNMENT_OPERATOR,
    IDENTIFIER_OPERATOR,
    CLOSE_EXPRESSION_OPERATOR,
    CONSTANT,
};

const std::unordered_set<TokenType> BINARY_OPERATIONS = {
        TokenType::OR_OPERATOR,
        TokenType::AND_OPERATOR,
        TokenType::EQUALITY,
        TokenType::IMPLICATION,
};

bool isBinaryOperation(const TokenType &token_type) {
    return BINARY_OPERATIONS.find(token_type) != BINARY_OPERATIONS.end();
}


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
        case TokenType::IMPLICATION:
            os << "implication";
            return os;
        case TokenType::EQUALITY:
            os << "equality";
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

    bool operator==(const Token &other) const
    { return (this->id == other.id
              && this->position == other.position
              && this->value == other.value
              && this->type == other.type
              );
    }
};

template<>
struct std::hash<Token> {
    std::size_t operator()(const Token &t) const {
        using std::size_t;
        using std::hash;
        using std::string;

        return ((hash<string>()(t.value)
                 ^ (hash<int>()(t.id) << 1)
                 ^ (hash<int>()(t.position) << 1)));
    }
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "token type: " << token.type << ", at position: " << token.position << ", with value: " << token.value;
    return os;
}


#endif //BOOLEAN_EXPRESSION_COMPILER_TOKEN_H
