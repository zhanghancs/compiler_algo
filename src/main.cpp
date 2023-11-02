/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 15:12:34
 * @FilePath: /algo_ds/src/main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#include "RegularParser.hpp"
#include <regex>

int main()
{   //
    RegularParser regularParser("aa*(a|b)*");
    Nfa           nfa = regularParser.Regular2Nfa();
    nfa.print();
    Dfa dfa(nfa);
    dfa.print();

    Dfa minDfa = dfa.minimize();
    minDfa.print();

    // std::regex re("aa*(bab*a)*(a|b)b*");
    // std::cout << std::boolalpha << std::regex_match("ababbabbb", re) << std::endl;
    // std::cout << std::boolalpha << dfa.Match("ababbabbb") << std::endl;

    return 0;
}