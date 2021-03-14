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

class PDNFParser {
private:
    std::shared_ptr<BooleanExpression> root;
    Lexer lexer;
    Token symbol;

public:
    explicit PDNFParser(const Lexer &lexer) : lexer(lexer) {}

    void build() { factor(); }

    void debug(std::ostream &os) {
        std::string rightEdge = "└──";
        auto terminal = std::dynamic_pointer_cast<Terminal>(root);
        if (terminal) {
            os << terminal->string() << "\n";
            return;
        }
        auto non_terminal = std::dynamic_pointer_cast<NonTerminal>(root);
        std::string leftEdge = (root != nullptr) ? "├──" : "└──";

        std::string res;
        res += root->string();
        auto right = non_terminal->GetRigth();
        traverseNodes(res, "", leftEdge, non_terminal->GetLeft(),
                      right != nullptr);
        traverseNodes(res, "", rightEdge, right, false);
        os << res;
    }

private:
    void binaryFormula() {
        unaryFormula();
        while (symbol == Token::OR_OPERATOR) {
            auto operation = std::make_shared<OrOperation>();
            operation->SetLeft(root);
            unaryFormula();
            operation->SetRight(root);
            root = operation;
        }
    }

    void unaryFormula() {
        factor();
        while (symbol == Token::AND_OPERATOR) {
            auto operation = std::make_shared<AndOperation>();
            operation->SetLeft(root);
            factor();
            operation->SetRight(root);
            root = operation;
        }
    }

    void factor() {
        symbol = lexer.GetNext();
        if (symbol == Token::NOT_OPERATOR) {
            auto not_op = std::make_shared<NotOperation>(NotOperation());
            factor();
            not_op->SetRight(root);
            root = not_op;
            symbol = lexer.GetNext();
        } else if (symbol == Token::OPEN_BRACKET) {
            binaryFormula();
            symbol = lexer.GetNext();
        } else if (symbol == Token::SYMBOL) {
            root = std::make_shared<Terminal>(Terminal(lexer.GetPrev()));
            symbol = lexer.GetNext();
        } else {
            throw std::invalid_argument("todo");
        }
    }

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
                (non_terminal->GetRigth() != nullptr) ? "├──" : "└──";

        traverseNodes(sb, new_padding, left_edge,
                      non_terminal->GetLeft(),
                      non_terminal->GetRigth() != nullptr);
        traverseNodes(sb, new_padding, right_edge,
                      non_terminal->GetRigth(), false);
    }
};

#endif //INC_1LAB_PARSER_H
