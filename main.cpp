#include <array>
#include <cassert>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>
#include "src/parser.h"
#include "src/semantic_analyzer.h"
#include "src/compiler.h"
#include <sstream>
#include <ranges>


void testParserDebug() {
    std::string expected = "OR\n├──OR\n│  ├──NOT\n│  │  └──A\n│  └──A\n└──OR\n   ├──NOT\n   │  └──A\n   └──A";
    Lexer lexer("( ((!A)|A)|((!A)|A))");
    auto parser = Parser(lexer);

    parser.build();
    std::stringstream os;
    parser.debug(os);
    assert(expected == os.str());
}

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


int main() {
    testParserDebug();
    auto test_cases = std::vector<std::pair<std::string, std::optional<std::string>>>{
            {"(((((!A)&B)&(!C))|(A&((!B)&(!C))))|((B&(!A))&(!C)))",        {"got equal elementary conjunction"}},
            {"((A&A)|(A&(!B)))",                                           "got repeated element: A"},
            {"((A&B)|(A&(!B)))",                                           {}},
            {"((A&B)|(A&(!B)))",                                           {}},
            {"(A&B)|(A&(!B)))",                                            "syntax error in your formula"},
            {"((A&B)|(A&(!(!B))))",                                        "expected a symbol here"},
            {"((A&(!B))|(A&(!B)))",                                        "got equal elementary conjunction"},
            {"(((!A)&B)|(A&(!B)))",                                        {}},
            {R"((((!A)/\B)\/(A/\(!B))))",                                  {}},
            {"(((!A)&B)|(A&(!B)))",                                        {}},
            {"(((((!A)&B)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {}},
            {"(((((!A)|B)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {"got not equal vars in conjunctions"}},
            {"(((((!A)&B)&((!C)&A))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))", {"got repeated element: A"}},
            {"(((((!A)&D)&(!C))|(A&((!B)&(!C))))|(((!A)&(!B))&(!C)))",     {"got not equal vars in conjunctions"}},
            {"",                                                           "unsupported token"},
            {" ",                                                          "unsupported token"},
            {"1",                                                          "expected or operator"},
            {"0",                                                          "expected or operator"},
    };
    std::ranges::for_each(test_cases, [&](auto pair) {
        Compiler compiler(pair.first);
        auto err_msg = compiler.IsPDNF();
        AssertEqual(pair.second, err_msg);
    });
    std::cout << "Tests passed!";
}