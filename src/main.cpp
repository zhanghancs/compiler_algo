/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 15:12:34
 * @FilePath: /algo_ds/src/main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#include "RegularParser.hpp"

int main()
{   // ((a|b)(a|b|c)*)|(c(b|c)*((a(b|c)*)|((b|c)*)))
    RegularParser regularParser("aa*(bab*a)*(a|b)b*");
    Nfa           nfa = regularParser.Regular2Nfa();
    nfa.print();
    Dfa dfa(nfa);
    dfa.print();

    Dfa minDfa = dfa.minimize();
    minDfa.print();

    std::cout << std::boolalpha << dfa.Match("ababbabbb") << std::endl;
    return 0;
}