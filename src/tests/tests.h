//
// Created by illfate on 3/17/21.
//

#ifndef BOOLEAN_EXPRESSION_COMPILER_TESTS_H
#define BOOLEAN_EXPRESSION_COMPILER_TESTS_H


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

void TestParserDebug() {
    std::string expected = "OR\n├──OR\n│  ├──NOT\n│  │  └──A\n│  └──A\n└──OR\n   ├──NOT\n   │  └──A\n   └──A";
    auto lexer = std::make_unique<Lexer>(Lexer("( ((!A)|A)|((!A)|A))"));
    auto parser = Parser(std::move(lexer));

    parser.build();
    std::stringstream os;
    parser.debug(os);
    AssertEqual(expected, os.str());
}

void RunTableTests() {
    auto test_cases = std::vector<std::pair<std::string, std::optional<std::string>>>{
        {"(((((!A)&B)&(!C))|(A&((!B)&(!C))))|((B&(!A))&(!C)))",        {"got equal elementary conjunction"}},
        {"((A&A)|(A&(!B)))",                                           "got repeated element: A"},
        {"((A&B)|(A&(!B)))",                                           {}},
        {"((A&B)|(A&(!B)))A",                                           "syntax error in your formula, unexpected identifier: token type: type, at position: 17, with value: A"},
        {"((A&B)|(A&(!B))) A",                                           "syntax error in your formula, unexpected identifier: token type: type, at position: 18, with value: A"},
        {"((A&B)|(A&(!B)))\nA",                                           "syntax error in your formula, unexpected identifier: token type: type, at position: 18, with value: A"},
        {"((A&B)|(A&(!B)))",                                           {}},
        {"(A&B)|(A&(!B)))",                                            "syntax error in your formula, unexpected identifier: token type: or operator, at position: 6, with value: |"},
        {"((A&B)|(A&(!(!B))))",                                        "expected a type here"},
        {"((A&(!B))|(A&(!B)))",                                        "got equal elementary conjunction"},
        {"(((!A)&B)|(A&(!B)))",                                        {}},
        {R"((((!A)/\B)\/(A/\(!B))))",                                  {}},
        {"(((!A)&B)|(A&(!B)))",                                        {}},
        {"(((((!A)&B)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {}},
        {"(((((!A)|B)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {"got not equal vars in conjunctions"}},
        {"(((((!A)&B)&((!C)&A))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))", {"got repeated element: A"}},
        {"(((((!A)&D)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {"got not equal vars in conjunctions"}},
        {"",                                                           "unexpected type"},
        {" ",                                                          "unexpected eof"},
        {"1",                                                          "expected or operator"},
        {"0",                                                          "expected or operator"},
    };
    std::ranges::for_each(test_cases, [&](auto pair) {
        auto str = std::string(pair.first);
        Compiler compiler(str);
        auto err_msg = compiler.IsPDNF();
        AssertEqual(pair.second, err_msg);
    });
}


void RunTests() {
    TestParserDebug();
    RunTableTests();
    std::cout << "Tests passed!\n";
}

#endif //BOOLEAN_EXPRESSION_COMPILER_TESTS_H
