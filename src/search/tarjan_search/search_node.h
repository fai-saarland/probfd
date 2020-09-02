#pragma once

#include "../state_id.h"
#include "search_node_info.h"

class GlobalOperator;

namespace tarjan_search {

class SearchNode {
public:
    SearchNode();
    SearchNode(SearchNode&&) = default;
    SearchNode(const SearchNode&) = default;
    SearchNode(SearchNodeInfo* info, StateID state_id);
    SearchNode& operator=(const SearchNode& other) = default;
    bool is_new() const;
    bool is_closed() const;
    bool is_dead_end() const;
    bool is_recognized_dead_end() const;
    bool is_onstack() const;
    unsigned get_index() const;
    unsigned get_lowlink() const;
    int get_h() const;
    StateID get_state_id() const;
    StateID get_parent_state_id() const;
    const GlobalOperator* get_operator() const;

    void popped_from_stack();
    void mark_dead_end();
    void mark_recognized_dead_end();
    void open_initial_state(int h);
    void open(const SearchNode& parent, const GlobalOperator* op, int h);
    bool update_h(int h);
    void close(unsigned index);
    bool update_lowlink(unsigned index);

private:
    SearchNodeInfo* info_;
    StateID state_id_;
};

} // namespace tarjan_search
