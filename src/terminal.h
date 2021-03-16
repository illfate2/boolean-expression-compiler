#include <utility>

//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_TERMINAL_H
#define INC_1LAB_TERMINAL_H


class Terminal : public BooleanExpression {
public:
    Terminal(std::string str) : symbol(std::move(str)) {}

    void interpret() const override {}

    std::string string() const { return symbol; }

    TokenType getTokenType() const override {
        return TokenType::SYMBOL;
    }

private:
    std::string symbol;
};

class Constant : public BooleanExpression {
public:
    Constant(const std::string &str) {
        if (str == "1") {
            value = true;
        } else {
            value = false;
        }
    }

    void interpret() const override {
        // TODO
    }

    std::string string() const override {
        return std::to_string(value);
    }

    TokenType getTokenType() const override {
        return TokenType::CONSTANT;
    }

private:
    bool value;
};


#endif //INC_1LAB_TERMINAL_H
