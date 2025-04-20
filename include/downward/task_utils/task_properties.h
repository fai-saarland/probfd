#ifndef TASK_UTILS_TASK_PROPERTIES_H
#define TASK_UTILS_TASK_PROPERTIES_H

#include "downward/per_task_information.h"
#include "downward/task_proxy.h"

#include "downward/algorithms/int_packer.h"

namespace downward::task_properties {
inline bool is_applicable(const AxiomOrOperatorProxy& op, const State& state)
{
    for (FactProxy precondition : op.get_preconditions()) {
        if (state[precondition.get_variable()] != precondition.get_value())
            return false;
    }
    return true;
}

inline bool is_applicable(const PartialOperatorProxy& op, const State& state)
{
    for (FactProxy precondition : op.get_preconditions()) {
        if (state[precondition.get_variable()] != precondition.get_value())
            return false;
    }
    return true;
}

inline bool is_goal_state(const PlanningTaskProxy& task, const State& state)
{
    for (FactProxy goal : task.get_goals()) {
        if (state[goal.get_variable()] != goal.get_value()) return false;
    }
    return true;
}

/*
  Return true iff all operators have cost 1.

  Runtime: O(n), where n is the number of operators.
*/
extern bool is_unit_cost(const TaskProxy& task);

// Runtime: O(1)
extern bool has_axioms(const PlanningTaskProxy& task);

/*
  Report an error and exit with ExitCode::UNSUPPORTED if the task has axioms.
  Runtime: O(1)
*/
extern void verify_no_axioms(const PlanningTaskProxy& task);

// Runtime: O(n), where n is the number of operators.
extern bool has_conditional_effects(const TaskProxy& task);

/*
  Report an error and exit with ExitCode::UNSUPPORTED if the task has
  conditional effects.
  Runtime: O(n), where n is the number of operators.
*/
extern void verify_no_conditional_effects(const TaskProxy& task);

extern std::vector<int> get_operator_costs(const TaskProxy& task_proxy);
extern double get_average_operator_cost(const TaskProxy& task_proxy);
extern int get_min_operator_cost(const TaskProxy& task_proxy);

/*
  Return the number of facts of the task.
  Runtime: O(n), where n is the number of state variables.
*/
extern int get_num_facts(const PlanningTaskProxy& task_proxy);

/*
  Return the total number of effects of the task, including the
  effects of axioms.
  Runtime: O(n), where n is the number of operators and axioms.
*/
extern int get_num_total_effects(const TaskProxy& task_proxy);

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

extern void print_variable_statistics(const PlanningTaskProxy& task_proxy);
extern void dump_pddl(const PlanningTaskProxy& task_proxy, const State& state);
extern void dump_fdr(const VariablesProxy& variables, const State& state);
extern void dump_goals(const GoalsProxy& goals);
extern void dump_task(const TaskProxy& task_proxy);

extern PerTaskInformation<int_packer::IntPacker> g_state_packers;
} // namespace downward::task_properties

#endif
