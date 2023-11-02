
/*
 * @Author: zhanghan zhangh.coder@outlook.com
 * @Date: 2023-10-25 14:22:57
 * @FilePath: /compiler_algo/include/Graph.hpp
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
    // enum TYPE
    // {
    //     START,
    //     NORMAL,
    //     END,
    // };
    Node*  head_;
    size_t num_nodes_;
};