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
    // , dfa_id_(0)
    {}
    ~RegularParser(){

    };

    Nfa Regular2Nfa()
    {
        std::stack<char> op;
        std::stack<Nfa>  val;
        size_t           pre_type = 0;
        for (size_t i = 0; i < regular_.size(); ++i) {
            if (std::isalpha(regular_[i])) {
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
        // nfa_ = val.top();
        return val.top();
    }

    // Dfa Nfa2Dfa()
    // {
    //     std::vector<Node*>            dfaNode;
    //     std::vector<std::set<size_t>> dtran;
    //     std::set<size_t>              T    = nfa_.EpsilonClosure(nfa_.head()->id());
    //     Node*                         node = new Node(dfa_id_++, Dfa::TYPE::START);
    //     size_t                        now  = 0;

    //     dtran.push_back(T);
    //     dfaNode.push_back(node);

    //     while (now < dfa_id_) {
    //         T = dtran[now];
    //         for (const char& c : alphabet_set_) {
    //             std::set<size_t> next = nfa_.EpsilonClosure(nfa_.Move(T, c));
    //             Dfa::TYPE        type = Dfa::TYPE::NORMAL;
    //             if (!next.empty()) {
    //                 if (next.find(nfa_.tail()->id()) != next.end())
    //                     type = Dfa::TYPE::END;
    //                 else
    //                     type = Dfa::TYPE::NORMAL;
    //                 int next_pos = is_in_dtran(dtran, next);
    //                 if (next_pos == -1) {
    //                     dtran.push_back(next);
    //                     node = new Node(dfa_id_++, type);
    //                     dfaNode[now]->add_next(node, c);
    //                     dfaNode.push_back(node);
    //                 }
    //                 else {
    //                     dfaNode[now]->add_next(dfaNode[next_pos], c);
    //                 }
    //             }
    //         }
    //         ++now;
    //     }
    //     dfa_ = Dfa(dfaNode[0], dfa_id_);
    //     return dfa_;
    // }

private:
    std::string regular_;
    size_t      nfa_id_;
    // size_t      dfa_id_;
    // Nfa         nfa_;
    // Dfa         dfa_;

    // const std::vector<char> alphabet_set_ = {'a', 'b', 'c'};

    enum PRIORITY
    {
        NONE,
        LEFT,
        RIGHT,
        OR,
        AND,
        CLOSURE,
    };


    // int is_in_dtran(std::vector<std::set<size_t>>& dtran, std::set<size_t>& T)
    // {
    //     for (size_t i = 0; i < dtran.size(); ++i) {
    //         if (T == dtran[i]) return int(i);
    //     }
    //     return -1;
    // }

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
        return Nfa(lhs.head(), rhs.tail(), lhs.num_nodes() + rhs.num_nodes());
    }

    Nfa RegularOr(Nfa& lhs, Nfa& rhs)
    {
        Node* head = new Node(nfa_id_++);
        Node* tail = new Node(nfa_id_++);
        head->add_next(lhs.head(), '0');
        head->add_next(rhs.head(), '0');
        lhs.tail()->add_next(tail, '0');
        rhs.tail()->add_next(tail, '0');
        return Nfa(head, tail, lhs.num_nodes() + rhs.num_nodes() + 2);
    }

    Nfa RegularClosure(Nfa& lhs)
    {
        Node* head = new Node(nfa_id_++);
        Node* tail = new Node(nfa_id_++);
        head->add_next(lhs.head(), '0');
        head->add_next(tail, '0');
        lhs.tail()->add_next(tail, '0');
        lhs.tail()->add_next(lhs.head(), '0');
        return Nfa(head, tail, lhs.num_nodes() + 2);
    }
};
