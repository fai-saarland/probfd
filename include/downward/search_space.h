#ifndef SEARCH_SPACE_H
#define SEARCH_SPACE_H

#include "downward/operator_cost.h"
#include "downward/per_state_information.h"
#include "downward/search_node_info.h"

#include <vector>

namespace downward {
class OperatorProxy;
class State;
class AbstractTask;
}

namespace downward::utils {
class LogProxy;
}

namespace downward {
class SearchNode {
    State state;
    SearchNodeInfo& info;

public:
    SearchNode(const State& state, SearchNodeInfo& info);

    const State& get_state() const;

    bool is_new() const;
    bool is_open() const;
    bool is_closed() const;
    bool is_dead_end() const;

    int get_g() const;
    int get_real_g() const;

    void open_initial();
    void open(
        const SearchNode& parent_node,
        const OperatorProxy& parent_op,
        const OperatorIntCostFunction& cost_function,
        int adjusted_cost);
    void reopen(
        const SearchNode& parent_node,
        const OperatorProxy& parent_op,
        const OperatorIntCostFunction& cost_function,
        int adjusted_cost);
    void update_parent(
        const SearchNode& parent_node,
        const OperatorProxy& parent_op,
        const OperatorIntCostFunction& cost_function,
        int adjusted_cost);
    void close();
    void mark_as_dead_end();

    void dump(const AbstractTask& task, utils::LogProxy& log) const;
};

class SearchSpace {
    PerStateInformation<SearchNodeInfo> search_node_infos;

    StateRegistry& state_registry;
    utils::LogProxy& log;

public:
    SearchSpace(StateRegistry& state_registry, utils::LogProxy& log);

    SearchNode get_node(const State& state);
    void
    trace_path(const State& goal_state, std::vector<OperatorID>& path) const;

    void dump(const AbstractTask& task) const;
    void print_statistics() const;
};
}

#endif
