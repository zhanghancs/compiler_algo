#pragma once

#include "Nfa.hpp"
#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <unordered_map>

class Dfa : public Graph
{
public:
    Dfa(Node* head = nullptr, size_t num_nodes = 0)
        : Graph(head, num_nodes)
        , alphabet_set_({'a', 'b', 'c'})
    {}

    Dfa(Nfa& nfa)
        : alphabet_set_({'a', 'b', 'c'})
    {
        std::vector<Node*>            dfaNode;
        std::vector<std::set<size_t>> dtran;
        std::set<size_t>              T    = nfa.EpsilonClosure(nfa.head()->id());
        Node*                         node = new Node(num_nodes_++, Dfa::TYPE::START);
        size_t                        now  = 0;

        dtran.push_back(T);
        dfaNode.push_back(node);

        while (now < num_nodes_) {
            T = dtran[now];
            for (const char& c : alphabet_set_) {
                std::set<size_t> next = nfa.EpsilonClosure(nfa.Move(T, c));
                Dfa::TYPE        type = Dfa::TYPE::NORMAL;
                if (!next.empty()) {
                    if (next.find(nfa.tail()->id()) != next.end())
                        type = Dfa::TYPE::END;
                    else
                        type = Dfa::TYPE::NORMAL;
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
        return head->type() == END;
    }


    Dfa minimize()
    {
        std::map<std::set<size_t>, Node*> map;
        std::set<size_t>                  s, f;
        std::queue<Node*>                 q;
        std::vector<size_t>               flag(num_nodes_, 0);
        std::stack<std::set<size_t>>      st;
        size_t                            count = 0;
        std::vector<std::set<size_t>>     current_sets;

        q.push(head_);
        flag[head_->id()] = 1;
        while (!q.empty()) {
            Node* now = q.front();
            q.pop();
            if (now->type() == END)
                f.insert(now->id());
            else
                s.insert(now->id());
            for (std::pair<Node*, char>& next : now->next()) {
                if (flag[next.first->id()] == 0) {
                    q.push(next.first);
                    flag[next.first->id()] = 1;
                }
            }
        }
        st.push(s);
        st.push(f);
        current_sets.push_back(s);
        current_sets.push_back(f);

        while (!st.empty()) {
            std::set<size_t> now = st.top();
            st.pop();
            if (now.size() == 1) {
                map[now] = new Node(count++, NORMAL);
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
                    map[now] = new Node(count++, NORMAL);
                }
            }
        }
        Node* new_head = nullptr;
        for (auto& entry : map) {
            for (auto& id : entry.first) {
                if (get_node(id)->type() == END)
                    entry.second->set_type(END);
                else if (get_node(id)->type() == START) {
                    entry.second->set_type(START);
                    new_head = entry.second;
                }
            }
            for (const char& c : alphabet_set_) {
                Node* next_node = get_next(entry.first, c, map);
                if (next_node != nullptr) {
                    entry.second->add_next(next_node, c);
                }
            }
        }
        return Dfa(new_head, count);
    }

public:
    enum TYPE
    {
        START,
        NORMAL,
        END,
    };

private:
    std::vector<char> alphabet_set_;
    std::set<size_t>  tail_;

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