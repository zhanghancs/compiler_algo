/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-26 16:57:15
 * @FilePath: /algo_ds/include/Nfa.hpp
 * @Description:
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#pragma once

#include "Graph.hpp"
#include <set>
class Nfa : public Graph
{
public:
    Nfa(Node* head = nullptr, Node* tail = nullptr, size_t num_nodes = 0)
        : Graph(head, num_nodes)
        , tail_(tail)
    {}

    Nfa(char c, size_t& id)
    {
        head_ = new Node(id++);
        tail_ = new Node(id++);
        head_->add_next(tail_, c);
        num_nodes_ = 2;
    }

    ~Nfa(){};

    Node* tail() { return tail_; }

    bool Match(std::string str)
    {
        std::set<size_t> ans = EpsilonClosure(head_->id());
        for (char& c : str) {
            ans = EpsilonClosure(Move(ans, c));
        }
        return ans.find(tail_->id()) != ans.end();
    }


private:
    Node* tail_;

    std::set<size_t> next(size_t id, char c)
    {
        std::set<size_t> set;
        Node*            node = get_node(id);
        for (std::pair<Node*, char>& next : node->next()) {
            if (next.second == c) {
                set.insert(next.first->id());
            }
        }
        return set;
    }
};