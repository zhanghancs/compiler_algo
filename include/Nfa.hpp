/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-26 16:57:15
 * @FilePath: /compiler_algo/include/Nfa.hpp
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
    Nfa(Node* head = nullptr, size_t num_nodes = 0, Node* tail = nullptr)
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

    size_t Match(std::string str)
    {
        std::set<size_t> ans = EpsilonClosure(head_->id());
        for (char& c : str) {
            ans = EpsilonClosure(Move(ans, c));
            if (ans.empty()) return 0;
        }
        size_t res = 0;
        for (size_t id : ans) {
            res = std::max(get_node(id)->type(), res);
        }
        return res;
        // reverse traverse ans
        // for (std::set<size_t>::reverse_iterator rit = ans.rbegin(); rit != ans.rend(); ++rit) {
        //     // size_t id = *rit;
        //     // if (tails_.find(id) != tails_.end()) return get_node(id)->type();
        // }
    }

    std::set<char> get_alphabet_set() { return alphabet_set_; }
    void           set_alphabet_set(std::set<char>& alphabet_set) { alphabet_set_ = alphabet_set; }

    // void add_tails(size_t id) { tails_.insert(id); }

    // std::set<size_t> get_tails() { return tails_; }

    /**
     * @description: from id, find all the nodes that can be reached by epsilon
     * @param {size_t} id
     * @return {*}
     */
    std::set<size_t> EpsilonClosure(const size_t id)
    {
        std::set<size_t>   set;
        std::queue<size_t> q;
        std::vector<int>   flag(num_nodes_, 0);
        size_t             cur;
        q.push(id);
        flag[id] = 1;
        while (!q.empty()) {
            cur = q.front();
            q.pop();
            set.insert(cur);
            for (size_t next_node_id : next(cur, '0')) {
                if (flag[next_node_id] == 0) {
                    q.push(next_node_id);
                    flag[next_node_id] = 1;
                }
            }
        }
        return set;
    }

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

    /**
     * @description: from set T, find all the nodes that can be reached by char a
     * @param {set<size_t>} T
     * @param {char} a
     * @return {*}
     */
    std::set<size_t> Move(std::set<size_t>& T, char a)
    {
        std::set<size_t> set;
        for (size_t id : T) {
            std::set<size_t> seti = next(id, a);
            set.insert(seti.begin(), seti.end());
        }
        return set;
    }

    /**
     * @description: find all the nodes that can be reached by epsilon
     * @param {set<size_t>} T
     * @return {*}
     */
    std::set<size_t> EpsilonClosure(std::set<size_t> T)
    {
        std::set<size_t> set;
        std::set<size_t> epsilon_seti;
        for (const size_t& i : T) {
            epsilon_seti = EpsilonClosure(i);
            set.insert(epsilon_seti.begin(), epsilon_seti.end());
        }
        return set;
    }

private:
    std::set<char> alphabet_set_;
    Node*          tail_;
    // std::set<size_t> tails_;
};