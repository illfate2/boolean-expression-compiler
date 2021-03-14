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
    void SetRight(const std::shared_ptr<BooleanExpression>& right) {
        this->right = right;
    }

    void SetLeft(const std::shared_ptr<BooleanExpression>& left) {
        this->left = left;
    }

    const std::shared_ptr<BooleanExpression>& GetRigth() const { return right; }

    std::shared_ptr<BooleanExpression> GetLeft() const { return left; }
};

class OrOperation : public NonTerminal {
public:
    std::string string() const override { return "OR"; }

    void interpret() const override {}
};

class AndOperation : public NonTerminal {
public:
    void interpret() const override {}
    std::string string() const override { return "AND"; }
};

class NotOperation : public NonTerminal {
public:
    void interpret() const override {}
    std::string string() const override { return "NOT"; }
};


#endif //INC_1LAB_NON_TERMINAL_H
