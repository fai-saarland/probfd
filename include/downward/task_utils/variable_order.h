#ifndef TASK_UTILS_VARIABLE_ORDER_FINDER_H
#define TASK_UTILS_VARIABLE_ORDER_FINDER_H

#include "downward/abstract_task.h"
#include "downward/task_utils/causal_graph.h"

#include <functional>
#include <memory>
#include <vector>

namespace downward::utils {
class RandomNumberGenerator;
} // namespace downward::utils

namespace downward::variable_order {

class VariableFilter {
public:
    virtual ~VariableFilter() = default;

    virtual bool select(int var) = 0;

    virtual void notify_selected(int var) = 0;
};

std::unique_ptr<VariableFilter> create_variable_filter_trivial();

std::unique_ptr<VariableFilter> create_variable_filter_goal(
    const VariableSpace& variables,
    const GoalFactList& goals);

std::unique_ptr<VariableFilter> create_variable_filter_relation(
    const VariableSpace& variables,
    std::function<std::vector<int>(int)> variable_relation);

class VariableOrder {
    std::vector<int> remaining_vars;
    std::vector<std::unique_ptr<VariableFilter>> filters;

public:
    VariableOrder(
        std::vector<int> remaining_vars,
        std::vector<std::unique_ptr<VariableFilter>> filters);

    bool done() const;

    int next();
};

VariableOrder create_variable_order_level(const AbstractTaskTuple& task);

VariableOrder
create_variable_order_reverse_level(const AbstractTaskTuple& task);

VariableOrder create_variable_order_random(
    const AbstractTaskTuple& task,
    utils::RandomNumberGenerator& rng);

VariableOrder create_variable_order_cg_goal_level(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg);

VariableOrder create_variable_order_cg_goal_random(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg,
    utils::RandomNumberGenerator& rng);

VariableOrder create_variable_order_goal_cg_level(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg);

} // namespace downward::variable_order

#endif
