#include "probfd/task_utils/task_properties.h"

#include "probfd/probabilistic_task.h"

#include "downward/task_utils/task_properties.h"

#include "downward/initial_state_values.h"
#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include "downward/operator_cost_function.h"
#include "probfd/probabilistic_operator_space.h"
#include "probfd/termination_costs.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <print>
#include <ranges>
#include <unordered_map>
#include <utility>

using namespace std;
namespace vws = std::views;

using namespace downward;

using utils::ExitCode;

namespace probfd::task_properties {

bool is_applicable(const ProbabilisticOperatorProxy& op, const State& state)
{
    for (const auto [var, value] : op.get_preconditions()) {
        if (state[var] != value) return false;
    }
    return true;
}

value_t get_adjusted_action_cost(
    const ProbabilisticOperatorProxy& op,
    const OperatorCostFunction<value_t>& cost_function,
    OperatorCost cost_type,
    bool is_unit_cost)
{
    return get_adjusted_action_cost(
        op.get_id(),
        cost_function,
        cost_type,
        is_unit_cost);
}

value_t get_adjusted_action_cost(
    int op_index,
    const downward::OperatorCostFunction<value_t>& cost_function,
    downward::OperatorCost cost_type,
    bool is_unit_cost)
{
    switch (cost_type) {
    case NORMAL: return cost_function.get_operator_cost(op_index);
    case ONE: return 1_vt;
    case PLUSONE:
        if (is_unit_cost)
            return 1_vt;
        else
            return cost_function.get_operator_cost(op_index) + 1_vt;
    default: throw utils::CriticalError("Unknown cost type");
    }
}

bool is_unit_cost(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function)
{
    for (const auto op : operators) {
        if (cost_function.get_operator_cost(op.get_id()) != 1_vt) return false;
    }

    return true;
}

static int
get_first_conditional_effects_op_id(const ProbabilisticOperatorSpace& operators)
{
    for (ProbabilisticOperatorProxy op : operators) {
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            for (ProbabilisticEffectProxy effect : outcome.get_effects()) {
                if (!effect.get_conditions().empty()) return op.get_id();
            }
        }
    }
    return -1;
}

bool has_conditional_effects(const ProbabilisticOperatorSpace& operators)
{
    return get_first_conditional_effects_op_id(operators) != -1;
}

void verify_no_conditional_effects(const ProbabilisticOperatorSpace& operators)
{
    int op_id = get_first_conditional_effects_op_id(operators);
    if (op_id != -1) {
        throw utils::UnsupportedError(
            "This configuration does not support conditional effects "
            "(operator {})!",
            operators[op_id].get_name());
    }
}

vector<value_t> get_operator_costs(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function)
{
    vector<value_t> costs;
    costs.reserve(operators.size());
    for (ProbabilisticOperatorProxy op : operators)
        costs.push_back(cost_function.get_operator_cost(op.get_id()));
    return costs;
}

value_t get_average_operator_cost(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function)
{
    value_t average_operator_cost = 0;
    for (ProbabilisticOperatorProxy op : operators) {
        average_operator_cost += cost_function.get_operator_cost(op.get_id());
    }
    return average_operator_cost / static_cast<value_t>(operators.size());
}

value_t get_min_operator_cost(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function)
{
    value_t min_cost = INFINITE_VALUE;
    for (ProbabilisticOperatorProxy op : operators) {
        min_cost = min(min_cost, cost_function.get_operator_cost(op.get_id()));
    }
    return min_cost;
}

value_t get_cost_lower_bound(
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function,
    const TerminationCosts& termination_costs)
{
    return get_min_operator_cost(operators, cost_function) >= 0_vt
               ? std::min(
                     termination_costs.get_goal_termination_cost(),
                     termination_costs.get_non_goal_termination_cost())
               : -INFINITE_VALUE;
}

int get_num_total_effects(
    const AxiomSpace& axioms,
    const ProbabilisticOperatorSpace& operators)
{
    int num_effects = 0;
    for (ProbabilisticOperatorProxy op : operators)
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes())
            num_effects += outcome.get_effects().size();
    num_effects += axioms.size();
    return num_effects;
}

namespace {

void dump_probabilistic_task_(const ProbabilisticTaskTuple& task, auto& os)
{
    const auto& [variables, axioms, operators, goals, init_vals, cost_function, _] =
        task;

    value_t min_action_cost = numeric_limits<int>::max();
    value_t max_action_cost = 0;

    for (ProbabilisticOperatorProxy op : operators) {
        min_action_cost =
            min(min_action_cost, cost_function.get_operator_cost(op.get_id()));
        max_action_cost =
            max(max_action_cost, cost_function.get_operator_cost(op.get_id()));
    }

    println(os, "Min action cost: {}", min_action_cost);
    println(os, "Max action cost: {}", max_action_cost);

    println(os, "Variables ({}):", variables.size());
    for (VariableProxy var : variables) {
        println(os, "  {} (range {})", var.get_name(), var.get_domain_size());
        for (int val = 0; val < var.get_domain_size(); ++val) {
            println(os, "    {}: {}", val, var.get_fact(val).get_name());
        }
    }
    State initial_state = init_vals.get_initial_state();
    println(os, "Initial state (PDDL):");
    ::task_properties::dump_pddl(variables, initial_state);
    println(os, "Initial state (FDR):");
    ::task_properties::dump_fdr(variables, initial_state);
    ::task_properties::dump_goals(variables, goals);
}

} // namespace

void dump_probabilistic_task(
    const ProbabilisticTaskTuple& task,
    utils::LogProxy& log)
{
    dump_probabilistic_task_(task, log);
}

void dump_probabilistic_task(
    const ProbabilisticTaskTuple& task,
    std::ostream& os)
{
    dump_probabilistic_task_(task, os);
}

} // namespace probfd::task_properties
