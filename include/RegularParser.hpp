/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 14:20:15
 * @FilePath: /compiler_algo/include/RegularParser.hpp
 * @Description:
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#pragma once

#include "Dfa.hpp"
#include "Nfa.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

class RegularParser
{
public:
    RegularParser(std::string regular_)
        : regular_(regular_)
        , nfa_id_(0)
    {}

    RegularParser(std::vector<std::string> regulars_)
        : regulars_(regulars_)
        , nfa_id_(0)
    {}

    ~RegularParser(){

    };

    Nfa Regular2Nfa(size_t id = 1)
    {
        std::stack<char> op;
        std::stack<Nfa>  val;
        size_t           pre_type = 0;
        for (size_t i = 0; i < regular_.size(); ++i) {
            if (std::isalpha(regular_[i])) {
                alphabet_set_.insert(regular_[i]);
                if (pre_type == 1) add_op(op, val, '&');
                val.push(Nfa(regular_[i], nfa_id_));
                pre_type = 1;
            }
            else {
                if (regular_[i] == '|')
                    pre_type = 0;
                else if (regular_[i] == '(') {
                    pre_type = 0;
                    if (i != 0 && regular_[i - 1] != '|' && regular_[i - 1] != '(') {
                        add_op(op, val, '&');
                    }
                }
                else
                    pre_type = 1;
                add_op(op, val, regular_[i]);
            }
        }
        while (!op.empty()) {
            calc(op, val);
        }
        assert(val.size() == 1);
        nfa_ = val.top();
        nfa_.tail()->set_type(id);
        nfa_.set_alphabet_set(alphabet_set_);
        // nfa_.add_tails(nfa_.tail()->id());
        return nfa_;
    }

    Nfa Regulars2Nfa()
    {
        std::vector<Nfa> nfas;
        for (size_t i = 0; i < regulars_.size(); ++i) {
            regular_ = regulars_[i];
            nfas.push_back(Regular2Nfa(i + 1));
        }
        Node* head = new Node(nfa_id_++);
        nfa_       = Nfa(head, nfa_id_, nullptr);
        for (Nfa& i : nfas) {
            head->add_next(i.head(), '0');
            // nfa_.add_tails(i.tail()->id());
        }
        nfa_.set_alphabet_set(alphabet_set_);
        return nfa_;
    }

private:
    std::string              regular_;
    std::vector<std::string> regulars_;
    size_t                   nfa_id_;
    Nfa                      nfa_;
    std::set<char>           alphabet_set_;

    enum PRIORITY
    {
        NONE,
        LEFT,
        RIGHT,
        OR,
        AND,
        CLOSURE,
    };


    void calc(std::stack<char>& op, std::stack<Nfa>& val)
    {
        char new_op = op.top();
        if (new_op == '*') {
            Nfa val1 = val.top();
            val.pop();
            val.push(RegularClosure(val1));
        }
        else {
            Nfa val2 = val.top();
            val.pop();
            Nfa val1 = val.top();
            val.pop();
            if (new_op == '&') {
                val.push(RegularAnd(val1, val2));
            }
            else if (new_op == '|') {
                val.push(RegularOr(val1, val2));
            }
            else {
                std::cerr << "RegularParser::calc: error" << std::endl;
            }
        }
        op.pop();
    }

    void add_op(std::stack<char>& op, std::stack<Nfa>& val, char c)
    {
        PRIORITY pr = get_priority(c);
        if (pr != LEFT) {
            while (!op.empty() && get_priority(op.top()) >= pr) calc(op, val);
        }
        if (pr == RIGHT)
            op.pop();
        else
            op.push(c);
    }

    PRIORITY get_priority(char& c)
    {
        switch (c) {
        case '(': return LEFT;
        case ')': return RIGHT;
        case '|': return OR;
        case '&': return AND;
        case '*': return CLOSURE;
        default: return NONE;
        }
    }

    Nfa RegularAnd(Nfa& lhs, Nfa& rhs)
    {
        lhs.tail()->add_next(rhs.head(), '0');
        return Nfa(lhs.head(), lhs.num_nodes() + rhs.num_nodes(), rhs.tail());
    }

    Nfa RegularOr(Nfa& lhs, Nfa& rhs)
    {
        Node* head = new Node(nfa_id_++);
        Node* tail = new Node(nfa_id_++);
        head->add_next(lhs.head(), '0');
        head->add_next(rhs.head(), '0');
        lhs.tail()->add_next(tail, '0');
        rhs.tail()->add_next(tail, '0');
        return Nfa(head, lhs.num_nodes() + rhs.num_nodes() + 2, tail);
    }

    Nfa RegularClosure(Nfa& lhs)
    {
        Node* head = new Node(nfa_id_++);
        Node* tail = new Node(nfa_id_++);
        nfa_id_++;
        head->add_next(lhs.head(), '0');
        head->add_next(tail, '0');
        lhs.tail()->add_next(tail, '0');
        lhs.tail()->add_next(lhs.head(), '0');
        return Nfa(head, lhs.num_nodes() + 2, tail);
    }
};
