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
    std::unique_ptr<Lexer> lexer;
    Token token;
    std::shared_ptr<SymbolTable> symbol_table;

public:
    explicit Parser(std::unique_ptr<Lexer> &&lexer, const std::shared_ptr<SymbolTable> &symbol_table) : lexer(
            std::move(lexer)), symbol_table(symbol_table) {}

    void build() {
        factor();
        if (!lexer->IsEmpty()) {
            try {
                token = lexer->GetNext();
            } catch (const std::exception &exception) {
                if (exception.what() == std::string("unexpected eof")) {
                    return;
                } else {
                    throw std::invalid_argument(exception.what());
                }
            }
            std::stringstream ss;
            ss << "syntax error in your formula, unexpected identifier: " << token;
            throw std::invalid_argument(ss.str());
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
        token = lexer->GetNext();
        assert((token.type == TokenType::OR_OPERATOR || token.type == TokenType::AND_OPERATOR) ||
               token.type == TokenType::IMPLICATION || token.type == TokenType::EQUALITY);
        parseBinaryFormula<OrOperation>();
        parseBinaryFormula<AndOperation>();
        parseBinaryFormula<ImplicationOperation>();
        parseBinaryFormula<EqualityOperation>();
    }

    template<typename Operation>
    void parseBinaryFormula() {
        Operation op;
        while (token.type == op.getTokenType()) {
            auto operation = std::make_shared<Operation>();
            operation->SetLeft(root);
            factor();
            token = lexer->GetNext();
            operation->SetRight(root);
            root = operation;
            match(token, TokenType::CLOSE_BRACKET);
        }
    }

    void unaryFormula() {
        lexer->GetNext();
        factor();
        auto not_op = std::make_shared<NotOperation>(NotOperation());
        not_op->SetChild(root);
        root = not_op;
    }

    void handleFormula() {
        TokenType next = lexer->LookupNext().type;
        if (next == TokenType::NOT_OPERATOR) {
            unaryFormula();
            token = lexer->GetNext();
        } else {
            binaryFormula();
        }
        match(token, TokenType::CLOSE_BRACKET);
    }

    void factor() {
        token = lexer->GetNext();
        if (token.type == TokenType::OPEN_BRACKET) {
            handleFormula();
        } else if (token.type == TokenType::SYMBOL) {
            root = std::make_shared<Terminal>(Terminal(token.value));
        } else if (token.type == TokenType::CONSTANT) {
            root = std::make_shared<Constant>(Constant(token.value));
        } else if (token.type == TokenType::IDENTIFIER_OPERATOR) {
            token = lexer->GetNext();
            match(token, TokenType::SYMBOL);

            auto symbol = std::move(token);
            token = lexer->GetNext();
            match(token, TokenType::ASSIGNMENT_OPERATOR);

            token = lexer->GetNext();
            match(token, TokenType::CONSTANT);
            auto const_token = std::move(token);
            token = lexer->GetNext();
            match(token, TokenType::CLOSE_EXPRESSION_OPERATOR);
            symbol_table->SetTokenToConst(symbol, Constant(const_token.value));
            factor();
        } else {
            std::stringstream ss;
            throw std::invalid_argument("unexpected type");
        }
    }

    void match(const Token &got, TokenType want) {
        if (got.type != want) {
            std::stringstream ss;
            ss << "unexpected type: " << "got: " << got << ", bur want: " << want << "\n";
            throw std::invalid_argument(ss.str());
        }
    }


};

#endif //INC_1LAB_PARSER_H
