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

template<std::ranges::range R>
auto to_vector(R &&r) {
    auto r_common = r | std::views::common;
    return std::vector(r_common.begin(), r_common.end());
}


class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(std::shared_ptr<BooleanExpression> root) : root(std::move(root)) {}

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
            checkForEqualVars(sorted_parsed_dnfs);
            if (parsed_dnfs.size() > std::pow(2, parsed_dnfs[0].size())) {
                throw std::invalid_argument("got to many conjunction");
            }
        } catch (const std::exception &ex) {
            return std::optional(ex.what());
        }
        return {};
    }

private:

    std::vector<std::vector<std::pair<std::string, bool>>> parseFormula() {
        splitDNFs(root);
        std::vector<std::vector<std::pair<std::string, bool>>> parsed_dnfs;
        parsed_dnfs.resize(dnfs.size());
        size_t counter = 0;
        for (const auto &dnf:dnfs) {
            isDNF(dnf, parsed_dnfs[counter]);
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

    template<typename It, typename BackIT>
    void findDuplicatesWithOutput(It begin, It end, BackIT back_inserter) {
        std::sort(begin, end);

        std::set<typename std::iterator_traits<It>::value_type> unique(begin, end);

        std::set_difference(begin, end, unique.begin(), unique.end(), back_inserter);
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
        std::stringstream ss;
        ss << "got repeated element: ";
        std::move(diff.begin(), diff.end(), std::ostream_iterator<std::string>(ss));
        throw std::invalid_argument(ss.str());
    }

    void isDNF(const std::shared_ptr<BooleanExpression> &node, std::vector<std::pair<std::string, bool>> &parsed_dnf) {
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
            isDNF(node, parsed_dnf);
        } else if (right_token_type == TokenType::NOT_OPERATOR) {
            auto not_operation = std::dynamic_pointer_cast<NotOperation>(node);
            if (not_operation->GetChild()->getTokenType() != TokenType::SYMBOL) {
                throw std::invalid_argument("expected a type here");
            }
            parsed_dnf.emplace_back(not_operation->GetChild()->string(), false);
        }
    }

    void splitDNFs(const std::shared_ptr<BooleanExpression> &node) {
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
            throw std::invalid_argument("unexpected");
        }
    }

    std::shared_ptr<BooleanExpression> root;
    std::vector<std::shared_ptr<BooleanExpression>> dnfs;

};

#endif //INC_1LAB_SEMANTIC_ANALYZER_H
