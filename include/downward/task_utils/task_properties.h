#ifndef TASK_UTILS_TASK_PROPERTIES_H
#define TASK_UTILS_TASK_PROPERTIES_H

#include "downward/axiom_utils.h"
#include "downward/goal_fact_list.h"
#include "downward/per_task_information.h"
#include "downward/state.h"

#include "downward/algorithms/int_packer.h"

namespace downward {
template <typename>
class OperatorCostFunction;
}

namespace downward::task_properties {

inline bool is_applicable(const AxiomOrOperatorProxy& op, const State& state)
{
    for (const auto [var, value] : op.get_preconditions()) {
        if (state[var] != value) return false;
    }
    return true;
}

inline bool is_applicable(const OperatorProxy& op, const State& state)
{
    for (const auto [var, value] : op.get_preconditions()) {
        if (state[var] != value) return false;
    }
    return true;
}

inline bool is_applicable(const PartialOperatorProxy& op, const State& state)
{
    for (const auto [var, value] : op.get_preconditions()) {
        if (state[var] != value) return false;
    }
    return true;
}

inline bool is_goal_state(const GoalFactList& goals, const State& state)
{
    for (const auto [var, value] : goals) {
        if (state[var] != value) return false;
    }
    return true;
}

/*
  Return true iff all operators have cost 1.

  Runtime: O(n), where n is the number of operators.
*/
extern bool is_unit_cost(
    const OperatorSpace& operators,
    const OperatorCostFunction<int>& cost_function);

// Runtime: O(1)
extern bool has_axioms(const AxiomSpace& axiom_space);

/*
  Report an error and exit with ExitCode::UNSUPPORTED if the task has axioms.
  Runtime: O(1)
*/
extern void verify_no_axioms(const AxiomSpace& task);

// Runtime: O(n), where n is the number of operators.
extern bool has_conditional_effects(const ClassicalOperatorSpace& task);

/*
  Report an error and exit with ExitCode::UNSUPPORTED if the task has
  conditional effects.
  Runtime: O(n), where n is the number of operators.
*/
extern void verify_no_conditional_effects(const ClassicalOperatorSpace& task);

extern std::vector<int> get_operator_costs(
    const OperatorSpace& operators,
    const OperatorCostFunction<int>& cost_function);

extern double get_average_operator_cost(
    const OperatorSpace& operators,
    const OperatorCostFunction<int>& cost_function);

extern int get_min_operator_cost(
    const OperatorSpace& operators,
    const OperatorCostFunction<int>& cost_function);

/*
  Return the number of facts of the task.
  Runtime: O(n), where n is the number of state variables.
*/
extern int get_num_facts(const VariableSpace& var_space);

/*
  Return the total number of effects of the task, including the
  effects of axioms.
  Runtime: O(n), where n is the number of operators and axioms.
*/
extern int get_num_total_effects(
    const AxiomSpace& axioms,
    const ClassicalOperatorSpace& task);

template <std::ranges::input_range FactProxyCollection>
    requires std::convertible_to<
        FactProxy,
        std::ranges::range_reference_t<FactProxyCollection>>
std::vector<FactPair> get_fact_pairs(const FactProxyCollection& facts)
{
    std::vector<FactPair> fact_pairs;
    fact_pairs.reserve(facts.size());
    for (FactProxy fact : facts) { fact_pairs.push_back(fact.get_pair()); }
    return fact_pairs;
}

template <std::ranges::input_range FactPairCollection>
    requires std::convertible_to<
        FactPair,
        std::ranges::range_reference_t<FactPairCollection>>
std::vector<FactPair> get_fact_pairs(const FactPairCollection& facts)
{
    std::vector<FactPair> fact_pairs;
    fact_pairs.reserve(facts.size());
    for (FactPair fact : facts) { fact_pairs.push_back(fact); }
    return fact_pairs;
}

extern void print_variable_statistics(
    const VariableSpace& variables,
    const int_packer::IntPacker& state_packer,
    std::ostream& out);

extern void dump_pddl(
    const VariableSpace& variables,
    const State& state,
    std::ostream& out);

extern void
dump_fdr(const VariableSpace& variables, const State& state, std::ostream& out);

extern void dump_goals(
    const VariableSpace& variables,
    const GoalFactList& goals,
    std::ostream& out);

extern void dump_task(const AbstractTaskTuple& task, std::ostream& out);

extern PerComponentInformation<int_packer::IntPacker, VariableSpace>
    g_state_packers;
} // namespace downward::task_properties

#endif
