//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_TERMINAL_H
#define INC_1LAB_TERMINAL_H


class Terminal : public BooleanExpression {
public:
    Terminal(char ch) { symbol = ch; }
    void interpret() const override {  }
    std::string string() const { return {symbol}; }

private:
    char symbol;
};


#endif //INC_1LAB_TERMINAL_H
