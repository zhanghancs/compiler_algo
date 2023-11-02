
/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 14:22:57
 * @FilePath: /algo_ds/include/Graph.hpp
 * @Description: the basic class of Nfa and Dfa, a graph class
 *
 * Copyright (c) 2023 by zhanghan, All Rights Reserved.
 */
#pragma once

#include "Node.hpp"
#include <cstdint>
#include <iostream>
#include <queue>
#include <set>
#include <stack>

class Graph
{
public:
    Graph(Node* head = NULL, size_t num_nodes = 0)
        : head_(head)
        , num_nodes_(num_nodes)
    {}

    ~Graph(){};

    void set_head(Node* head) { head_ = head; }
    // void set_tail(Node* tail) { tail_ = tail; }

    Node* head() { return head_; }

    size_t num_nodes() { return num_nodes_; }

    void set_num_nodes(size_t num_nodes) { num_nodes_ = num_nodes; }

    Node* get_node(size_t id)
    {
        std::queue<Node*> q;
        std::vector<int>  flag(num_nodes_, 0);
        q.push(head_);
        flag[head_->id()] = 1;
        while (!q.empty()) {
            Node* node = q.front();
            if (node->id() == id) return node;
            q.pop();
            for (auto next : node->next()) {
                if (flag[next.first->id()] == 0) {
                    q.push(next.first);
                    flag[next.first->id()] = 1;
                }
            }
        }
        return nullptr;
    }

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
     * @description: from node now, find the node that can be reached by char c, this only use in
     * dfa
     * @param {Node*} now
     * @param {char} c
     * @return {*}
     */
    Node* Move(Node* now, char c)
    {
        for (std::pair<Node*, char>& next : now->next()) {
            if (next.second == c) {
                return next.first;
            }
        }
        return nullptr;
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


    void print()
    {
        std::queue<std::pair<Node*, char>> q;
        std::vector<int>                   flag(num_nodes_, 0);
        q.push({head_, 's'});
        q.push({NULL, '\0'});
        flag[head_->id()] = 1;
        while (!q.empty()) {
            std::pair<Node*, char> node = q.front();
            q.pop();
            if (node.first == NULL) {
                std::cout << std::endl;
                if (!q.empty()) q.push({NULL, '\0'});
                continue;
            }
            else {
                for (std::pair<Node*, char>& next : node.first->next()) {
                    if (flag[next.first->id()] == 0) {
                        q.push(next);
                        flag[next.first->id()] = 1;
                    }
                    std::cout << node.first->id() << "(" << node.first->type() << ")"
                              << "--" << next.second << "-->" << next.first->id() << "("
                              << next.first->type() << ")"
                              << " ";
                }
            }
        }
        std::cout << std::endl;
    }

protected:
    Node*            head_;
    size_t           num_nodes_;
    std::set<size_t> tail_;
};