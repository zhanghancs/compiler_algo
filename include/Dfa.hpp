#pragma once

#include "Nfa.hpp"
#include <algorithm>
#include <cstddef>
#include <map>
#include <set>

class Dfa : public Graph
{
public:
    Dfa(Node* head = nullptr, size_t num_nodes = 0, std::set<char> alphabet_set = {})
        : Graph(head, num_nodes)
        , alphabet_set_(alphabet_set)
    {}

    Dfa(Nfa& nfa)
        : alphabet_set_(nfa.get_alphabet_set())
    {
        std::vector<Node*>            dfaNode;
        std::vector<std::set<size_t>> dtran;
        std::set<size_t>              T    = nfa.EpsilonClosure(nfa.head()->id());
        Node*                         node = new Node(num_nodes_++, 0);
        size_t                        now  = 0;
        std::set<size_t>              left_types;
        // std::set<size_t>              f    = nfa.get_tails();

        dtran.push_back(T);
        dfaNode.push_back(node);

        while (now < num_nodes_) {
            T = dtran[now];
            for (const char& c : alphabet_set_) {
                std::set<size_t> next = nfa.EpsilonClosure(nfa.Move(T, c));
                size_t           type = 0;
                if (!next.empty()) {
                    for (size_t i : next) {
                        type = std::max(nfa.get_node(i)->type(), type);
                        left_types.insert(type);
                    }
                    int next_pos = is_in_dtran(dtran, next);
                    if (next_pos == -1) {
                        dtran.push_back(next);
                        node = new Node(num_nodes_++, type);
                        dfaNode[now]->add_next(node, c);
                        dfaNode.push_back(node);
                    }
                    else {
                        dfaNode[now]->add_next(dfaNode[next_pos], c);
                    }
                }
            }
            ++now;
        }
        types = left_types.size();
        head_ = dfaNode[0];
    }

    ~Dfa(){};

    bool Match(std::string str)
    {
        Node* head = head_;
        for (char& c : str) {
            head = Move(head, c);
            if (head == nullptr) return false;
        }
        return head->type() != 0;
    }


    Dfa minimize()
    {

        std::queue<Node*>   q;
        std::vector<size_t> flag(num_nodes_, 0);

        std::map<std::set<size_t>, Node*> map;
        std::stack<std::set<size_t>>      st;
        size_t                            count = 0;
        std::vector<std::set<size_t>>     current_sets(types);

        flag[head_->id()] = 1;
        q.push(head_);
        while (!q.empty()) {
            Node* now = q.front();
            q.pop();
            current_sets[now->type()].insert(now->id());
            for (std::pair<Node*, char>& next : now->next()) {
                if (flag[next.first->id()] == 0) {
                    q.push(next.first);
                    flag[next.first->id()] = 1;
                }
            }
        }
        for (auto& i : current_sets) {
            st.push(i);
        }

        while (!st.empty()) {
            std::set<size_t> now = st.top();
            st.pop();
            if (now.size() == 1) {
                map[now] = new Node(count++, get_node(*now.begin())->type());
            }
            else {
                bool is_same = true;
                // 判断当前的 set 是否所有的 next_id 都在同一个 set 中
                for (const char& c : alphabet_set_) {
                    std::vector<std::set<size_t>> ans = divide(now, c, current_sets);
                    if (ans.size() != 1) {
                        is_same = false;
                        current_sets.erase(
                            std::remove(current_sets.begin(), current_sets.end(), now),
                            current_sets.end());
                        for (auto& i : ans) {
                            st.push(i);
                            current_sets.push_back(i);
                        }
                        break;
                    }
                }
                if (is_same) {
                    size_t type = 0;
                    for (auto& i : now) {
                        type = std::max(type, get_node(i)->type());
                    }
                    map[now] = new Node(count++, type);
                }
            }
        }
        Node* new_head = nullptr;
        for (auto& entry : map) {
            for (auto& id : entry.first) {
                if (id == head_->id()) {
                    new_head = entry.second;
                    break;
                }
            }
            for (const char& c : alphabet_set_) {
                Node* next_node = get_next(entry.first, c, map);
                if (next_node != nullptr) {
                    entry.second->add_next(next_node, c);
                }
            }
        }
        return Dfa(new_head, count, alphabet_set_);
    }

private:
    std::set<char> alphabet_set_;
    size_t         types;

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

    int is_in_dtran(std::vector<std::set<size_t>>& dtran, std::set<size_t>& T)
    {
        for (size_t i = 0; i < dtran.size(); ++i) {
            if (T == dtran[i]) return int(i);
        }
        return -1;
    }

    std::vector<std::set<size_t>> divide(std::set<size_t>& set, char c,
                                         std::vector<std::set<size_t>> cureent_sets)
    {
        std::vector<std::set<size_t>> res;
        std::map<size_t, int>         map;
        std::set<size_t>              nullSet;
        for (size_t element : set) {
            map[element] = get_id(Move(get_node(element), c));
            if (map[element] == -1) {
                nullSet.insert(element);
            }
        }
        for (auto& currentSet : cureent_sets) {
            std::set<size_t> resultSet;
            for (auto element : map) {
                if (currentSet.count(element.second) > 0) {
                    resultSet.insert(element.first);
                }
            }
            if (!resultSet.empty()) {
                res.push_back(resultSet);
            }
        }
        if (!nullSet.empty()) {
            res.push_back(nullSet);
        }
        return res;
    }

    Node* get_next(std::set<size_t> set, char c, std::map<std::set<size_t>, Node*>& res)
    {
        std::set<size_t> next_set;
        int              id          = *set.begin();
        std::set<size_t> next_id_set = get_node(id)->next(c);
        if (next_id_set.empty()) return nullptr;
        for (auto& entry : res) {
            if (std::includes(entry.first.begin(),
                              entry.first.end(),
                              next_id_set.begin(),
                              next_id_set.end())) {
                return entry.second;
            }
        }
        return nullptr;
    }

    int get_id(Node* node)
    {
        if (nullptr == node) return -1;
        return node->id();
    }
};