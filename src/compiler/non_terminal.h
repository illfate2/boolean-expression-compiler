//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_NON_TERMINAL_H
#define INC_1LAB_NON_TERMINAL_H

#include "expression.h"

class NonTerminal : public BooleanExpression {
protected:
    std::shared_ptr<BooleanExpression> left;
    std::shared_ptr<BooleanExpression> right;

public:
    NonTerminal() {}

    virtual std::string string() const = 0;

    void interpret() const override {
        this->left->interpret();
        this->right->interpret();
    }

    void SetRight(const std::shared_ptr<BooleanExpression> &right) {
        this->right = right;
    }

    void SetLeft(const std::shared_ptr<BooleanExpression> &left) {
        this->left = left;
    }

    const std::shared_ptr<BooleanExpression> &GetRight() const { return right; }

    const std::shared_ptr<BooleanExpression> &GetLeft() const { return left; }

    virtual TokenType getTokenType() const = 0;
};

class OrOperation : public NonTerminal {
public:
    std::string string() const override { return "OR"; }

    void interpret() const override {}

    TokenType getTokenType() const override {
        return TokenType::OR_OPERATOR;
    }
};

class ImplicationOperation : public NonTerminal {
public:
    std::string string() const override { return "IMPLICATION"; }

    void interpret() const override {}

    TokenType getTokenType() const override {
        return TokenType::IMPLICATION;
    }
};

class EqualityOperation : public NonTerminal {
public:
    std::string string() const override { return "EQUALITY"; }

    void interpret() const override {}

    TokenType getTokenType() const override {
        return TokenType::EQUALITY;
    }
};

class AndOperation : public NonTerminal {
public:
    void interpret() const override {}

    std::string string() const override { return "AND"; }

    TokenType getTokenType() const override {
        return TokenType::AND_OPERATOR;
    }
};

class NotOperation : public NonTerminal {
public:
    void interpret() const override {}

    std::string string() const override { return "NOT"; }

    TokenType getTokenType() const override {
        return TokenType::NOT_OPERATOR;
    }

    void SetChild(const std::shared_ptr<BooleanExpression> &child) {
        NonTerminal::SetRight(child);
    }

    void SetLeft(const std::shared_ptr<BooleanExpression> &left) {
        throw std::invalid_argument("unsupported");
    }

    void SetRight(const std::shared_ptr<BooleanExpression> &right) {
        throw std::invalid_argument("unsupported");
    }

    const std::shared_ptr<BooleanExpression> &GetRight() const { throw std::invalid_argument("unsupported"); }


    const std::shared_ptr<BooleanExpression> &GetChild() const { return NonTerminal::GetRight(); }

    const std::shared_ptr<BooleanExpression> &GetLeft() const { throw std::invalid_argument("unsupported"); }


};


#endif //INC_1LAB_NON_TERMINAL_H
