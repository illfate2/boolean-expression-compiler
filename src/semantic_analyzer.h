//
// Created by illfate on 3/14/21.
//

#ifndef INC_1LAB_SEMANTIC_ANALYZER_H
#define INC_1LAB_SEMANTIC_ANALYZER_H

#include <memory>
#include <vector>
#include "expression.h"
#include <ranges>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <functional>

template<std::ranges::range R>
auto to_vector(R &&r) {
    auto r_common = r | std::views::common;
    return std::vector(r_common.begin(), r_common.end());
}


class SemanticAnalyzer {
public:
    SemanticAnalyzer(const std::shared_ptr<BooleanExpression> &root) : root(root) {}

    std::optional<std::string> Is() {
        try {
            is(root);
            std::vector<std::vector<std::pair<std::string, bool>>> parsed_dnfs;
            parsed_dnfs.resize(dnfs.size());
            size_t counter = 0;
            for (const auto &dnf:dnfs) {
                isDNF(dnf, parsed_dnfs[counter]);
                assertNoRepeatedVars(parsed_dnfs[counter]);
                ++counter;
            }
            auto sorted_parsed_dnfs = to_vector(parsed_dnfs | std::ranges::views::transform([&](auto dnf) {
                std::ranges::sort(dnf.begin(), dnf.end(), [&](auto lhs, auto rhs) {
                    return lhs.first < rhs.first;
                });
                return dnf;
            }));
            if (hasDuplicates(sorted_parsed_dnfs)) {
                throw std::invalid_argument("got equal elementary conjunction");
            }
            for (const auto &dnf:sorted_parsed_dnfs) {
                if (dnf.size() != sorted_parsed_dnfs[0].size()) {
                    throw std::invalid_argument("got not equal vars in conjunctions");
                }
                for (size_t i = 1; i < dnf.size(); ++i) {
                    if (dnf[i].first != sorted_parsed_dnfs[0][i].first) {
                        throw std::invalid_argument("got not equal vars in conjunctions");
                    }
                }
            }
            // TODO check all the same args
            if (parsed_dnfs.size() > std::pow(2, parsed_dnfs[0].size())) {
                throw std::invalid_argument("got to many conjunction");
            }
        } catch (const std::exception &ex) {
            return std::optional(ex.what());
        }
        return {};
    }

    template<class T>
    bool hasDuplicates(std::vector<T> &v) {
        std::sort(v.begin(), v.end());

        return std::adjacent_find(v.begin(), v.end()) != v.end();
    }

    template<typename It, typename BackIT>
    void findDuplicatesWithOutput(It begin, It end, BackIT back_inserter) {
        std::sort(begin, end);

        std::set<typename std::iterator_traits<It>::value_type> uvec(begin, end);

        std::set_difference(begin, end, uvec.begin(), uvec.end(), back_inserter);
    }

    void assertNoRepeatedVars(const std::vector<std::pair<std::string, bool>> &parsed_dnf) {
        auto transformed = to_vector(parsed_dnf | std::ranges::views::transform([&](auto p) {
            return p.first;
        }));


        std::vector<std::string> diff;
        findDuplicatesWithOutput(transformed.begin(), transformed.end(), std::back_inserter(diff));
        if (diff.begin() == diff.end()) {
            return;
        }
        std::string err_msg("got repeated element: " + diff[0]); // TODO enhance
        throw std::invalid_argument(err_msg);
    }

    void isDNF(const std::shared_ptr<BooleanExpression> &node, std::vector<std::pair<std::string, bool>> &parsed_dnf) {
        auto and_operation = std::dynamic_pointer_cast<AndOperation>(node);
        auto left_node = and_operation->GetLeft();
        TokenType left_token_type = and_operation->GetLeft()->getTokenType();
        if (left_token_type == TokenType::SYMBOL) {
            parsed_dnf.emplace_back(and_operation->GetLeft()->string(), true);
        } else if (left_token_type == TokenType::AND_OPERATOR) {
            isDNF(and_operation->GetLeft(), parsed_dnf);
        } else if (left_token_type == TokenType::NOT_OPERATOR) {
            auto not_operation = std::dynamic_pointer_cast<NotOperation>(left_node);
            if (not_operation->GetRight()->getTokenType() != TokenType::SYMBOL) {
                throw std::invalid_argument("expected a symbol here");
            }
            parsed_dnf.emplace_back(not_operation->GetRight()->string(), false);
        }

        auto right_node = and_operation->GetRight();
        TokenType right_token_type = and_operation->GetRight()->getTokenType();
        if (right_token_type == TokenType::SYMBOL) {
            parsed_dnf.emplace_back(and_operation->GetRight()->string(), true);
        } else if (right_token_type == TokenType::AND_OPERATOR) {
            isDNF(and_operation->GetRight(), parsed_dnf);
        } else if (right_token_type == TokenType::NOT_OPERATOR) {
            auto not_operation = std::dynamic_pointer_cast<NotOperation>(right_node);
            if (not_operation->GetRight()->getTokenType() != TokenType::SYMBOL) {
                throw std::invalid_argument("expected a symbol here");
            }
            parsed_dnf.emplace_back(not_operation->GetRight()->string(), false);
        }
    }

    void debug(std::ostream &os) {
        for (const auto &dnf:dnfs) {
            debugNode(os, dnf);
            os << std::endl;
        }

    }

private:
    void is(const std::shared_ptr<BooleanExpression> &node) {
        if (node->getTokenType() != TokenType::OR_OPERATOR) {
            throw std::invalid_argument("expected or operator");
        }
        auto or_operation = std::dynamic_pointer_cast<OrOperation>(node);
        auto left = or_operation->GetLeft();
        if (left->getTokenType() == TokenType::OR_OPERATOR) {
            is(left);
        } else if (left->getTokenType() == TokenType::AND_OPERATOR) {
            dnfs.push_back(left);
        } else {
            throw std::invalid_argument("unexpected");
        }
        // same
        auto right = or_operation->GetRight();
        if (right->getTokenType() == TokenType::OR_OPERATOR) {
            is(right);
        } else if (right->getTokenType() == TokenType::AND_OPERATOR) {
            dnfs.push_back(right);
        } else {
            throw std::invalid_argument("unexpected");
        }
    }

    std::shared_ptr<BooleanExpression> root;
    std::vector<std::shared_ptr<BooleanExpression>> dnfs;

};

#endif //INC_1LAB_SEMANTIC_ANALYZER_H
