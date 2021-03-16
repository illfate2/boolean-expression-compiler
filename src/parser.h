//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_PARSER_H
#define INC_1LAB_PARSER_H

#include <stdexcept>
#include <iostream>
#include "non_terminal.h"
#include "terminal.h"
#include "lexer.h"

void traverseNodes(std::string &sb, const std::string &padding,
                   const std::string &edge,
                   const std::shared_ptr<BooleanExpression> &node,
                   bool has_right) {
    if (!node) {
        return;
    }
    sb += "\n" + padding + edge + node->string();

    std::string new_padding(padding);
    if (has_right) {
        new_padding.append("│  ");
    } else {
        new_padding.append("   ");
    }

    auto terminal = std::dynamic_pointer_cast<Terminal>(node);
    if (terminal) {
        return;
    }
    auto non_terminal = std::dynamic_pointer_cast<NonTerminal>(node);

    std::string right_edge = "└──";
    std::string left_edge =
            (non_terminal->GetRight() != nullptr) ? "├──" : "└──";

    traverseNodes(sb, new_padding, left_edge,
                  non_terminal->GetLeft(),
                  non_terminal->GetRight() != nullptr);
    traverseNodes(sb, new_padding, right_edge,
                  non_terminal->GetRight(), false);
}

void debugNode(std::ostream &os, const std::shared_ptr<BooleanExpression> &node) {
    std::string rightEdge = "└──";
    auto terminal = std::dynamic_pointer_cast<Terminal>(node);
    if (terminal) {
        os << terminal->string() << "\n";
        return;
    }
    auto non_terminal = std::dynamic_pointer_cast<NonTerminal>(node);
    std::string leftEdge = (node != nullptr) ? "├──" : "└──";

    std::string res;
    res += node->string();
    auto right = non_terminal->GetRight();
    traverseNodes(res, "", leftEdge, non_terminal->GetLeft(),
                  right != nullptr);
    traverseNodes(res, "", rightEdge, right, false);
    os << res;
}

class Parser {
private:
    std::shared_ptr<BooleanExpression> root;
    Lexer lexer;
    TokenType symbol;

public:
    explicit Parser(const Lexer &lexer) : lexer(lexer) {}

    void build() {
        factor();
        if (!lexer.IsEmpty()) {
            throw std::invalid_argument("syntax error in your formula");
        }
    }

    void debug(std::ostream &os) {
        debugNode(os, root);
    }

    std::shared_ptr<BooleanExpression> GetRoot() const {
        return root;
    }

private:
    void binaryFormula() {
        factor();
        assert((symbol == TokenType::OR_OPERATOR || symbol == TokenType::AND_OPERATOR));
        while (symbol == TokenType::OR_OPERATOR) {
            auto operation = std::make_shared<OrOperation>();
            operation->SetLeft(root);
            factor();
            operation->SetRight(root);
            root = operation;
            match(symbol, TokenType::CLOSE_BRACKET);
        }
        while (symbol == TokenType::AND_OPERATOR) {
            auto operation = std::make_shared<AndOperation>();
            operation->SetLeft(root);
            factor();
            operation->SetRight(root);
            root = operation;
            match(symbol, TokenType::CLOSE_BRACKET);
        }
    }

    void unaryFormula() {
        lexer.GetNext();
        factor();
        auto not_op = std::make_shared<NotOperation>(NotOperation());
        not_op->SetRight(root);
        root = not_op;
    }

    void handleFormula() {
        TokenType next = lexer.LookupNext().token;
        if (next == TokenType::NOT_OPERATOR) {
            unaryFormula();
        } else {
            binaryFormula();
        }
        match(symbol, TokenType::CLOSE_BRACKET);
        symbol = lexer.GetNext().token;
    }

    void factor() {
        Token token = lexer.GetNext();
        symbol = token.token;
        if (symbol == TokenType::OPEN_BRACKET) {
            handleFormula();
        } else if (symbol == TokenType::SYMBOL) {
            root = std::make_shared<Terminal>(Terminal(token.value));
            symbol = lexer.GetNext().token;
        } else if (symbol == TokenType::CONSTANT) {
            root = std::make_shared<Constant>(Constant(token.value));
            symbol = lexer.GetNext().token;
        } else {
            throw std::invalid_argument("unsupported token");
        }
    }

    void match(TokenType got, TokenType want) {
        assert(got == want);
    }


};

#endif //INC_1LAB_PARSER_H
