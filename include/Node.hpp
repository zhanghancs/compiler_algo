#pragma once

#include <cstdint>
#include <set>
#include <vector>

class Node
{
public:
    Node(size_t id, size_t type = 0)
        : id_(id)
        , type_(type)
    {}
    ~Node(){};

    size_t id() { return id_; }

    size_t type() { return type_; }

    void set_type(size_t type) { type_ = type; }

    std::vector<std::pair<Node*, char>> next() { return next_; }

    std::set<size_t> next(char c)
    {
        std::set<size_t> next;
        for (std::pair<Node*, char>& i : next_) {
            if (i.second == c) {
                next.insert(i.first->id());
            }
        }
        return next;
    }

    void add_next(Node* next, char value) { next_.push_back({next, value}); }

private:
    size_t                              id_;
    size_t                              type_;
    std::vector<std::pair<Node*, char>> next_;
};