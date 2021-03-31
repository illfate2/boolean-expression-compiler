//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_SEMANTIC_ANALYZER_H
#define INC_1LAB_SEMANTIC_ANALYZER_H

#include <memory>
#include <utility>
#include <vector>
#include "expression.h"
#include <ranges>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <functional>
#include <set>
#include <deque>

template<std::ranges::range R>
auto to_vector(R &&r) {
    auto r_common = r | std::views::common;
    return std::vector(r_common.begin(), r_common.end());
}


class SemanticAnalyzer {
public:
    SemanticAnalyzer(
            std::shared_ptr<BooleanExpression> root,
            const std::shared_ptr<SymbolTable> &symbol_table
    ) : root(std::move(root)), symbol_table(symbol_table) {}

    std::optional<std::string> IsPDNF() {
        try {
            auto parsed_dnfs = parseFormula();
            auto sorted_parsed_dnfs = to_vector(parsed_dnfs | std::ranges::views::transform([&](auto dnf) {
                std::ranges::sort(dnf.begin(), dnf.end(), [&](auto lhs, auto rhs) {
                    return lhs.first < rhs.first;
                });
                return dnf;
            }));
            if (hasDuplicates(sorted_parsed_dnfs)) {
                throw std::invalid_argument("got equal elementary conjunction");
            }
            if (parsed_dnfs.size() > std::pow(2, parsed_dnfs[0].size())) {
                throw std::invalid_argument("got to many conjunction");
            }
            checkForEqualVars(sorted_parsed_dnfs);
        } catch (const std::exception &ex) {
            return std::optional(ex.what());
        }
        return {};
    }

    struct FormulaResult {
        std::vector<std::string> symbols;
        std::deque<bool> results;
        std::vector<std::deque<bool>> matrix_values;
    };

    struct SharedComparator {
        template<typename T>
        bool operator()(const std::shared_ptr<T> &lhs,
                        const std::shared_ptr<T> &rhs) const {
            return (*lhs) < (*rhs);
        }
    };

    FormulaResult CalculateFormula() const {
        auto token_to_const = symbol_table->getTokenToConstant();
        std::vector<std::string> symbols;
        std::deque<bool> initial_values;
        for (const auto&[token, constant]:token_to_const) {
            symbols.push_back(token.value);
            initial_values.push_back(constant.getValue());
        }

        std::vector<std::shared_ptr<Terminal>> symbols_without_values;
        getSymbolsWithoutValuesAndSetValues(symbols_without_values, root, token_to_const);
        std::set<std::shared_ptr<Terminal>, SharedComparator> symbols_without_values_unique(
                symbols_without_values.begin(),
                symbols_without_values.end());
        for (const auto &term:symbols_without_values_unique) {
            symbols.push_back(term->string());
        }

        auto[matrix, results]=getRowsAndResult(symbols_without_values, symbols_without_values_unique, initial_values);
        return {
                .symbols=symbols,
                .results=results,
                .matrix_values=matrix
        };
    }

    std::pair<std::vector<std::deque<bool>>, std::deque<bool>>
    getRowsAndResult(const std::vector<std::shared_ptr<Terminal>> &symbols_without_values,
                     const std::set<std::shared_ptr<Terminal>, SharedComparator> &symbols_without_values_unique,
                     const std::deque<bool> &initial_values
    ) const {
        size_t amount_of_data_vectors = std::pow(2, symbols_without_values_unique.size());
        std::vector<std::deque<bool>> rows_of_symbol_values;
        std::deque<bool> results;
        for (size_t i = 0; i < amount_of_data_vectors; ++i) {
            std::deque<bool> row(initial_values.begin(), initial_values.end());
            auto it = symbols_without_values_unique.begin();
            for (size_t j = 0; j < symbols_without_values_unique.size(); ++j) {
                bool value = ((i & (int) std::pow(2, j)) > 0);
                row.push_back(value);
                for (auto &s:symbols_without_values) {
                    if (s->string() == it->get()->string()) {
                        s->SetValue(value);
                    }
                }
                ++it;
            }
            bool result = root->interpret();
            results.push_back(result);
            rows_of_symbol_values.push_back(std::move(row));
        }
        return {rows_of_symbol_values, results};
    }

    void
    getSymbolsWithoutValuesAndSetValues(
            std::vector<std::shared_ptr<Terminal>> &terminals,
            const std::shared_ptr<BooleanExpression> &expression,
            const std::unordered_map<Token, Constant> &term_to_constant) const {
        TokenType root_token_type = expression->getTokenType();
        if (isBinaryOperation(root_token_type)) {
            auto bin_op = std::dynamic_pointer_cast<NonTerminal>(expression);
            auto right = bin_op->GetRight();
            getSymbolsWithoutValuesAndSetValues(terminals, right, term_to_constant);
            auto left = bin_op->GetLeft();
            getSymbolsWithoutValuesAndSetValues(terminals, left, term_to_constant);
        } else if (root_token_type == TokenType::NOT_OPERATOR) {
            auto not_op = std::dynamic_pointer_cast<NotOperation>(expression);
            auto child = not_op->GetChild();
            getSymbolsWithoutValuesAndSetValues(terminals, child, term_to_constant);
        } else if (root_token_type == TokenType::SYMBOL) {
            auto symbol = std::dynamic_pointer_cast<Terminal>(expression);
            bool found = false;
            for (auto &[token, constant]:term_to_constant) {
                if (token.value == symbol->string()) {
                    found = true;
                    symbol->SetValue(constant.getValue());
                }
            }
            if (!found) {
                terminals.push_back(symbol);
            }
        }
    }

private:

    std::vector<std::vector<std::pair<std::string, bool>>> parseFormula() {
        splitDNFs(root);
        std::vector<std::vector<std::pair<std::string, bool>>> parsed_dnfs;
        parsed_dnfs.resize(dnfs.size());
        size_t counter = 0;
        for (const auto &dnf:dnfs) {
            checkIsDNF(dnf, parsed_dnfs[counter]);
            assertNoRepeatedVars(parsed_dnfs[counter]);
            ++counter;
        }
        return parsed_dnfs;
    }

    void checkForEqualVars(const std::vector<std::vector<std::pair<std::string, bool>>> &parsed_dnfs) const {
        for (const auto &dnf:parsed_dnfs) {
            if (dnf.size() != parsed_dnfs[0].size()) {
                throw std::invalid_argument("got not equal vars in conjunctions");
            }
            for (size_t i = 1; i < dnf.size(); ++i) {
                if (dnf[i].first != parsed_dnfs[0][i].first) {
                    throw std::invalid_argument("got not equal vars in conjunctions");
                }
            }
        }
    }

    template<class T>
    bool hasDuplicates(std::vector<T> &v) {
        std::sort(v.begin(), v.end());

        return std::adjacent_find(v.begin(), v.end()) != v.end();
    }

    template<typename It, typename BackIt>
    void findDuplicates(It begin, It end, BackIt back_inserter) {
        std::sort(begin, end);
        std::set<typename std::iterator_traits<It>::value_type> unique(begin, end);
        std::set_difference(begin, end, unique.begin(), unique.end(), back_inserter);
    }

    void assertNoRepeatedVars(const std::vector<std::pair<std::string, bool>> &parsed_dnf) {
        auto transformed = to_vector(parsed_dnf | std::ranges::views::transform([&](auto p) {
            return p.first;
        }));
        std::vector<std::string> diff;
        findDuplicates(transformed.begin(), transformed.end(), std::back_inserter(diff));
        if (diff.begin() == diff.end()) {
            return;
        }
        std::stringstream ss;
        ss << "got repeated element: ";
        std::move(diff.begin(), diff.end(), std::ostream_iterator<std::string>(ss));
        throw std::invalid_argument(ss.str());
    }

    void checkIsDNF(const std::shared_ptr<BooleanExpression> &node, std::vector<std::pair<std::string, bool>> &parsed_dnf) {
        auto and_operation = std::dynamic_pointer_cast<AndOperation>(node);
        checkNodeIsDNF(and_operation->GetLeft(), parsed_dnf);
        checkNodeIsDNF(and_operation->GetRight(), parsed_dnf);
    }

    void checkNodeIsDNF(const std::shared_ptr<BooleanExpression> &node,
                        std::vector<std::pair<std::string, bool>> &parsed_dnf) {
        TokenType right_token_type = node->getTokenType();
        if (right_token_type == TokenType::SYMBOL) {
            parsed_dnf.emplace_back(node->string(), true);
        } else if (right_token_type == TokenType::AND_OPERATOR) {
            checkIsDNF(node, parsed_dnf);
        } else if (right_token_type == TokenType::NOT_OPERATOR) {
            auto not_operation = std::dynamic_pointer_cast<NotOperation>(node);
            if (not_operation->GetChild()->getTokenType() != TokenType::SYMBOL) {
                throw std::invalid_argument("expected a type here");
            }
            parsed_dnf.emplace_back(not_operation->GetChild()->string(), false);
        } else {
            throw std::invalid_argument("unexpected token");
        }
    }

    void splitDNFs(const std::shared_ptr<BooleanExpression> &node) {
        if (node->getTokenType() == TokenType::AND_OPERATOR) {
            dnfs.push_back(node);
            return;
        }
        if (node->getTokenType() != TokenType::OR_OPERATOR) {
            throw std::invalid_argument("expected or operator");
        }
        auto or_operation = std::dynamic_pointer_cast<OrOperation>(node);
        auto left = or_operation->GetLeft();
        splitDNF(left);
        auto right = or_operation->GetRight();
        splitDNF(right);
    }

    void splitDNF(const std::shared_ptr<BooleanExpression> &node) {
        if (node->getTokenType() == TokenType::OR_OPERATOR) {
            splitDNFs(node);
        } else if (node->getTokenType() == TokenType::AND_OPERATOR) {
            dnfs.push_back(node);
        } else {
            std::stringstream ss;
            ss << "unexpected operator in PDNF: " << node->getTokenType();
            throw std::invalid_argument(ss.str());
        }
    }

    std::shared_ptr<BooleanExpression> root;
    std::vector<std::shared_ptr<BooleanExpression>> dnfs;
    std::shared_ptr<SymbolTable> symbol_table;

};

#endif //INC_1LAB_SEMANTIC_ANALYZER_H
