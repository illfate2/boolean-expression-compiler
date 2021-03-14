//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_EXPRESSION_H
#define INC_1LAB_EXPRESSION_H


#include <string>

class BooleanExpression {
public:
    virtual void interpret() const = 0;
    virtual std::string string() const = 0;
};

#endif //INC_1LAB_EXPRESSION_H
