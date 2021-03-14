#include <array>
#include <cassert>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>
#include "parser.h"



int main() {
    Lexer lexer("(((!A)&(!B))&(!C))|(((!A)&(!B))&C)|(((!A)&B)&(!C))|((A&B)&(!C))");
    auto parser = PDNFParser(lexer);

    parser.build();
    parser.debug(std::cout);
    // parser.debug();
    // assert(isPerfectDisjunctiveNormalForm(
    //    "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&b&!c)") ==
    //    true);  // правильный кейс
    // assert(isPerfectDisjunctiveNormalForm("(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&b)")
    // == false); // не хватает переменной
    // assert(isPerfectDisjunctiveNormalForm(
    //    "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&b&!c)|(a&b&!c)") ==
    //    false);  // повтор подформулы
    // assert(isPerfectDisjunctiveNormalForm(
    //    "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|!(a&b&!c)") == false);
    // assert(isPerfectDisjunctiveNormalForm("(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&a&b&!c)")
    // == false); // лишня переменная
    // assert(isPerfectDisjunctiveNormalForm("(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&!a&b&!c)")
    // == false); // лишняя переменная

    // assert(isPerfectDisjunctiveNormalForm("A|B") == true);
    // assert(isPerfectDisjunctiveNormalForm("(A&B)|!A") == true);
    // assert(isPerfectDisjunctiveNormalForm("(A&B&!C)|(!D&E&F)|(C&D)|B") ==
    // true);

    // assert(isPerfectDisjunctiveNormalForm("!(A|B)") == false);
    // assert(isPerfectDisjunctiveNormalForm("A|(B&(C|D))") == false);

    // assert(isPerfectDisjunctiveNormalForm("!A&!B")==true); TODO
    // assert(isPerfectDisjunctiveNormalForm("A|(B&C)|(B&D)")==true);
    // std::cout << "TESTS PASSED!\n";
    // "(!a&!b&!c)|(!a&!b&c)|(!a&b&!c)|(a&b&!c)"
}