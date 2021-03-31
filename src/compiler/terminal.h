#include <utility>

//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_TERMINAL_H
#define INC_1LAB_TERMINAL_H


class Terminal : public BooleanExpression {
public:
    Terminal(std::string str) : symbol(std::move(str)) {}

    bool interpret() const override {
        return value;
    }

    std::string string() const { return symbol; }

    TokenType getTokenType() const override {
        return TokenType::SYMBOL;
    }

    void SetValue(bool v) {
        value = v;
    }



private:
    std::string symbol;
    bool value;
};
bool operator<(const Terminal &lhs, const Terminal &rhs) {
    return lhs.string() < rhs.string();
}

class Constant : public BooleanExpression {
public:
    explicit Constant(const std::string &str) {
        if (str == "1") {
            value = true;
        } else {
            value = false;
        }
    }

    Constant &operator=(const Constant &con) {
        this->value = con.value;
        return *this;
    }

    bool interpret() const override {
        return value;
    }

    std::string string() const override {
        return std::to_string(value);
    }

    TokenType getTokenType() const override {
        return TokenType::CONSTANT;
    }

    bool getValue() const {
        return value;
    }

private:
    bool value;
};


#endif //INC_1LAB_TERMINAL_H
