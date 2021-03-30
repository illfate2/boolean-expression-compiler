//
// Created by illfate on 3/17/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_TESTS_CPP
#define BOOLEAN_EXPRESSION_COMPILER_TESTS_CPP

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch2/catch.hpp"
#include "../compiler/compiler.h"

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &opt) {
    if (opt) {
        os << opt.value();
    } else {
        os << "{}";
    }
    return os;
}

template<class T, class U>
void AssertEqual(const T &t, const U &u, const std::string &hint = {}) {
    if (t != u) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

TEST_CASE("Test formula calculation") {
    std::string formula = "let A=1; (A/\\B)";
    Compiler compiler(formula);
    auto result_var = compiler.CalculateFormula();
    auto result = std::get<SemanticAnalyzer::FormulaResult>(result_var);
    CHECK(result.symbols == std::vector<std::string>{"A", "B"});
    CHECK(result.results == std::deque<bool>{false, true});
    CHECK(result.matrix_values == std::vector<std::deque<bool>>{{true, false},
                                                                {true, true}});
}

TEST_CASE("Test repeated symbols calculation") {
    std::string formula = R"((((A/\A)/\A)/\D))";
    Compiler compiler(formula);
    auto result_var = compiler.CalculateFormula();
    auto result = std::get<SemanticAnalyzer::FormulaResult>(result_var);
    CHECK(result.symbols == std::vector<std::string>{"A", "D"});
    CHECK(result.results == std::deque<bool>{false, false, false, true});
    CHECK(result.matrix_values == std::vector<std::deque<bool>>{
            {false, false},
            {true,  false},
            {false, true},
            {true,  true},
    });
}

TEST_CASE("Test repeated symbols calculation with A=0") {
    std::string formula = R"( let A=0; (((A/\A)/\A)/\D))";
    Compiler compiler(formula);
    auto result_var = compiler.CalculateFormula();
    auto result = std::get<SemanticAnalyzer::FormulaResult>(result_var);
    CHECK(result.symbols == std::vector<std::string>{"A", "D"});
    CHECK(result.results == std::deque<bool>{false, false});
    CHECK(result.matrix_values == std::vector<std::deque<bool>>{
            {false, false},
            {false, true},
    });
}

template<typename T>
bool operator==(const std::vector<T> &lhs, const std::vector<T> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool operator==(const std::deque<T> &lhs, const std::deque<T> &rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}


TEST_CASE("Test parser debug") {
    std::string expected = "OR\n├──OR\n│  ├──NOT\n│  │  └──A\n│  └──A\n└──OR\n   ├──NOT\n   │  └──A\n   └──A";
    auto symbol_table = std::make_shared<SymbolTable>();
    auto lexer = std::make_unique<Lexer>(Lexer("( ((!A)\\/A)\\/((!A)\\/A))", symbol_table));
    auto parser = Parser(std::move(lexer), symbol_table);

    parser.build();
    std::stringstream os;
    parser.debug(os);
    REQUIRE(expected == os.str());
}

TEST_CASE("test compiler") {
    auto test_cases = std::vector<std::pair<std::string, std::optional<std::string>>>{
            {R"((((((!A)/\B)/\(!C))\/(A/\((!B)/\(!C))))\/((B/\(!A))/\(!C))))",         {"got equal elementary conjunction"}},
            {R"(((A/\A)\/(A/\(!B))))",                                                 "got repeated element: A"},
            {R"(((A/\B)\/(A/\(!B))))",                                                 {}},
            {R"(((A/\B)\/(A/\(!B)))A)",                                                "syntax error in your formula, unexpected identifier: token type: type, at position: 20, with value: A"},
            {R"(((A/\B)\/(A/\(!B))))",                                                 {}},
            {R"(((A/\B)\/(A/\(!(!B)))))",                                              "expected a type here"},
            {R"(((A/\(!B))\/(A/\(!B))))",                                              "got equal elementary conjunction"},
            {R"((((!A)/\B)\/(A/\(!B))))",                                              {}},
            {R"((((!A)/\B)\/(A/\(!B))))",                                              {}},
            {R"((((!A)/\B)\/(A/\(!B))))",                                              {}},
            {R"((((((!A)/\B)/\(!C))\/(A/\((!B)/\(!C))))\/(((!A)/\(!B))/\(!C))))",      {}},
            {R"((((((!A)\/B)/\(!C))\/(A/\((!B)/\(!C))))\/(((!A)/\(!B))/\(!C))))",      {"unexpected token"}},
            {R"((((((!A)/\B)/\((!C)/\A))\/(A/\((!B)/\(!C))))\/(((!A)/\(!B))/\(!C))))", {"got repeated element: A"}},
            {R"((((((!A)/\D)/\(!C))\/(A/\((!B)/\(!C))))\/(((!A)/\(!B))/\(!C))))",      {"got not equal vars in conjunctions"}},
            {"",                                                                       "unexpected type"},
            {" ",                                                                      "unexpected eof"},
            {"1",                                                                      "expected or operator"},
            {"0",                                                                      "expected or operator"},
            {"(A/\\B)",                                                                {}},
            {"(A/\\(!B))",                                                             {}},
            {"(A/\\(!A))",                                                             "got repeated element: A"},
    };
    std::ranges::for_each(test_cases, [&](auto pair) {
        auto str = std::string(pair.first);
        Compiler compiler(str);
        auto err_msg = compiler.IsPDNF();
        CHECK(pair.second == err_msg);
        if (pair.second != err_msg) {
            std::cerr << err_msg << "\n";
            std::cerr << pair.second << "\n";
        }
    });
}

#endif //BOOLEAN_EXPRESSION_COMPILER_TESTS_CPP
