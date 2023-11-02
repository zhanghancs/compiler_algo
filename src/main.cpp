/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 15:12:34
 * @FilePath: /compiler_algo/src/main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#include "RegularParser.hpp"

int main()
{
    std::vector<std::string> regulars = {
        // the higher index have a bigger priority
        "ab",
        "aaa*b",
    };
    RegularParser regularParsers(regulars);
    Nfa           nfa = regularParsers.Regulars2Nfa();
    nfa.print();
    std::cout << nfa.Match("aab") << std::endl;

    // RegularParser regularParser("ab");
    // Nfa           nfa = regularParser.Regular2Nfa();
    // nfa.print();
    // std::cout << nfa.Match("ab") << std::endl;

    Dfa dfa(nfa);
    dfa.print();

    Dfa minDfa = dfa.minimize();
    minDfa.print();

    // std::regex re("aa*(bab*a)*(a|b)b*");
    // std::cout << std::boolalpha << std::regex_match("ababbabbb", re) << std::endl;
    // std::cout << std::boolalpha << dfa.Match("ababbabbb") << std::endl;

    return 0;
}