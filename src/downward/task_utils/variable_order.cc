#include "downward/task_utils/variable_order.h"

#include "downward/task_utils/causal_graph.h"
#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/system.h"

#include "downward/abstract_task.h"
#include "downward/goal_fact_list.h"
#include "downward/variable_space.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

namespace vws = std::views;

using downward::utils::ExitCode;

namespace downward::variable_order {

namespace {

class VariableFilterTrivial : public VariableFilter {
public:
    bool select(int) override
    {
        return true;
    }

    void notify_selected(int) override
    {
    }
};

class VariableFilterGoal : public VariableFilter {
    std::vector<bool> is_goal_variable;

public:
    explicit VariableFilterGoal(
        const VariableSpace& variables,
        const GoalFactList& goals);

    bool select(int var) override;

    void notify_selected(int) override
    {
    }
};

class VariableFilterRelation : public VariableFilter {
    std::function<std::vector<int>(int)> variable_relation;
    std::vector<bool> is_relation_predecessor;

public:
    VariableFilterRelation(
        const VariableSpace& variables,
        causal_graph::CausalGraph cg);

    VariableFilterRelation(
        const VariableSpace& variables,
        std::function<std::vector<int>(int)> variable_relation);

    bool select(int var) override;

    void notify_selected(int var) override;
};

VariableFilterRelation::VariableFilterRelation(
    const VariableSpace& variables,
    causal_graph::CausalGraph cg)
    : variable_relation([cg = std::move(cg)](int var_no) {
        return cg.get_eff_to_pre(var_no);
    })
{
    const std::size_t var_count = variables.size();
    is_relation_predecessor.resize(var_count, false);
}

VariableFilterRelation::VariableFilterRelation(
    const VariableSpace& variables,
    std::function<std::vector<int>(int)> variable_relation)
    : variable_relation(std::move(variable_relation))
{
    const std::size_t var_count = variables.size();
    is_relation_predecessor.resize(var_count, false);
}

bool VariableFilterRelation::select(int var)
{
    return is_relation_predecessor[var];
}

void VariableFilterRelation::notify_selected(int var)
{
    for (const vector<int>& new_vars = variable_relation(var);
         const int new_var : new_vars) {
        is_relation_predecessor[new_var] = true;
    }
}

VariableFilterGoal::VariableFilterGoal(
    const VariableSpace& variables,
    const GoalFactList& goals)
{
    const std::size_t var_count = variables.size();
    is_goal_variable.resize(var_count, false);
    for (const FactPair goal : goals) is_goal_variable[goal.var] = true;
}

bool VariableFilterGoal::select(int var)
{
    return is_goal_variable[var];
}

} // namespace

std::unique_ptr<VariableFilter> create_variable_filter_trivial()
{
    return std::make_unique<VariableFilterTrivial>();
}

std::unique_ptr<VariableFilter> create_variable_filter_goal(
    const VariableSpace& variables,
    const GoalFactList& goals)
{
    return std::make_unique<VariableFilterGoal>(variables, goals);
}

std::unique_ptr<VariableFilter> create_variable_filter_relation(
    const VariableSpace& variables,
    std::function<std::vector<int>(int)> variable_relation)
{
    return std::make_unique<VariableFilterRelation>(
        variables,
        std::move(variable_relation));
}

VariableOrder::VariableOrder(
    std::vector<int> remaining_vars,
    std::vector<std::unique_ptr<VariableFilter>> filters)
    : remaining_vars(std::move(remaining_vars))
    , filters(std::move(filters))
{
}

bool VariableOrder::done() const
{
    return remaining_vars.empty();
}

int VariableOrder::next()
{
    assert(!done());

    for (const auto& filter : filters) {
        for (size_t i = 0; i < remaining_vars.size(); ++i) {
            const int var_no = remaining_vars[i];
            if (filter->select(var_no)) {
                for (const auto& filter2 : filters)
                    filter2->notify_selected(var_no);
                assert(remaining_vars[i] == var_no);
                remaining_vars.erase(remaining_vars.begin() + i);
                return var_no;
            }
        }
    }

    throw utils::InputError("No variable filter picked the variable!");
}

VariableOrder create_variable_order_level(const AbstractTaskTuple& task)
{
    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, get_variables(task).size()));

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(std::make_unique<VariableFilterTrivial>());

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

VariableOrder create_variable_order_reverse_level(const AbstractTaskTuple& task)
{
    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, get_variables(task).size()));

    std::ranges::reverse(remaining_vars);

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(std::make_unique<VariableFilterTrivial>());

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

VariableOrder create_variable_order_random(
    const AbstractTaskTuple& task,
    utils::RandomNumberGenerator& rng)
{
    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, get_variables(task).size()));

    rng.shuffle(remaining_vars);

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(std::make_unique<VariableFilterTrivial>());

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

VariableOrder create_variable_order_cg_goal_level(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg)
{
    const auto& variables = get_variables(task);
    const auto& goals = get_goal(task);

    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, variables.size()));

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(
        std::make_unique<VariableFilterRelation>(variables, cg));
    filters.emplace_back(
        std::make_unique<VariableFilterGoal>(variables, goals));

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

VariableOrder create_variable_order_cg_goal_random(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg,
    utils::RandomNumberGenerator& rng)
{
    const auto& variables = get_variables(task);
    const auto& goals = get_goal(task);

    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, variables.size()));

    rng.shuffle(remaining_vars);

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(
        std::make_unique<VariableFilterRelation>(variables, cg));
    filters.emplace_back(
        std::make_unique<VariableFilterGoal>(variables, goals));

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

VariableOrder create_variable_order_goal_cg_level(
    const AbstractTaskTuple& task,
    const causal_graph::CausalGraph& cg)
{
    const auto& variables = get_variables(task);
    const auto& goals = get_goal(task);

    std::vector<int> remaining_vars(
        std::from_range,
        vws::iota(0U, variables.size()));

    std::vector<std::unique_ptr<VariableFilter>> filters;
    filters.emplace_back(
        std::make_unique<VariableFilterGoal>(variables, goals));
    filters.emplace_back(
        std::make_unique<VariableFilterRelation>(variables, cg));

    return VariableOrder(std::move(remaining_vars), std::move(filters));
}

} // namespace downward::variable_order
