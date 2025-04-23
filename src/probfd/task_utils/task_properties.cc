#include "probfd/task_utils/task_properties.h"

#include "probfd/probabilistic_task.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/logging.h"
#include "downward/utils/system.h"

#include "downward/operator_cost_function.h"

#include <algorithm>
#include <iostream>
#include <limits>
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
    const downward::OperatorCostFunction<value_t>& cost_function,
    OperatorCost cost_type,
    bool is_unit_cost)
{
    switch (cost_type) {
    case NORMAL: return cost_function.get_operator_cost(op.get_id());
    case ONE: return 1_vt;
    case PLUSONE:
        if (is_unit_cost)
            return 1_vt;
        else
            return cost_function.get_operator_cost(op.get_id()) + 1_vt;
    default: ABORT("Unknown cost type");
    }
}

bool is_unit_cost(const ProbabilisticTask& task)
{
    for (const auto op : task.get_operators()) {
        if (task.get_operator_cost(op.get_id()) != 1_vt) return false;
    }

    return true;
}

static int get_first_conditional_effects_op_id(const ProbabilisticTask& task)
{
    for (ProbabilisticOperatorProxy op : task.get_operators()) {
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes()) {
            for (ProbabilisticEffectProxy effect : outcome.get_effects()) {
                if (!effect.get_conditions().empty()) return op.get_id();
            }
        }
    }
    return -1;
}

bool has_conditional_effects(const ProbabilisticTask& task)
{
    return get_first_conditional_effects_op_id(task) != -1;
}

void verify_no_conditional_effects(const ProbabilisticTask& task)
{
    int op_id = get_first_conditional_effects_op_id(task);
    if (op_id != -1) {
        ProbabilisticOperatorProxy op = task.get_operators()[op_id];
        cerr << "This configuration does not support conditional effects "
             << "(operator " << op.get_name() << ")!" << endl
             << "Terminating." << endl;
        utils::exit_with(ExitCode::SEARCH_UNSUPPORTED);
    }
}

vector<value_t> get_operator_costs(const ProbabilisticTask& task)
{
    vector<value_t> costs;
    ProbabilisticOperatorsProxy operators = task.get_operators();
    costs.reserve(operators.size());
    for (ProbabilisticOperatorProxy op : operators)
        costs.push_back(task.get_operator_cost(op.get_id()));
    return costs;
}

value_t get_average_operator_cost(const ProbabilisticTask& task)
{
    value_t average_operator_cost = 0;
    for (ProbabilisticOperatorProxy op : task.get_operators()) {
        average_operator_cost += task.get_operator_cost(op.get_id());
    }
    return average_operator_cost /
           static_cast<value_t>(task.get_operators().size());
}

value_t get_min_operator_cost(const ProbabilisticTask& task)
{
    return get_min_operator_cost(task.get_operators(), task);
}

value_t get_min_operator_cost(
    const ProbabilisticOperatorsProxy& ops,
    const OperatorCostFunction<value_t>& cost_function)
{
    value_t min_cost = INFINITE_VALUE;
    for (ProbabilisticOperatorProxy op : ops) {
        min_cost = min(min_cost, cost_function.get_operator_cost(op.get_id()));
    }
    return min_cost;
}

int get_num_total_effects(const ProbabilisticTask& task)
{
    int num_effects = 0;
    for (ProbabilisticOperatorProxy op : task.get_operators())
        for (ProbabilisticOutcomeProxy outcome : op.get_outcomes())
            num_effects += outcome.get_effects().size();
    num_effects += task.get_axioms().size();
    return num_effects;
}

namespace {

void dump_probabilistic_task_(const ProbabilisticTask& task, auto& os)
{
    ProbabilisticOperatorsProxy operators = task.get_operators();
    value_t min_action_cost = numeric_limits<int>::max();
    value_t max_action_cost = 0;
    for (ProbabilisticOperatorProxy op : operators) {
        min_action_cost =
            min(min_action_cost, task.get_operator_cost(op.get_id()));
        max_action_cost =
            max(max_action_cost, task.get_operator_cost(op.get_id()));
    }
    os << "Min action cost: " << min_action_cost << endl;
    os << "Max action cost: " << max_action_cost << endl;

    VariablesProxy variables = task.get_variables();
    os << "Variables (" << variables.size() << "):" << endl;
    for (VariableProxy var : variables) {
        os << "  " << var.get_name() << " (range " << var.get_domain_size()
           << ")" << endl;
        for (int val = 0; val < var.get_domain_size(); ++val) {
            os << "    " << val << ": " << var.get_fact(val).get_name() << endl;
        }
    }
    State initial_state = task.get_initial_state();
    os << "Initial state (PDDL):" << endl;
    ::task_properties::dump_pddl(task, initial_state);
    os << "Initial state (FDR):" << endl;
    ::task_properties::dump_fdr(variables, initial_state);
    ::task_properties::dump_goals(variables, task.get_goals());
}

} // namespace

void dump_probabilistic_task(
    const ProbabilisticTask& task,
    utils::LogProxy& log)
{
    dump_probabilistic_task_(task, log);
}

void dump_probabilistic_task(
    const ProbabilisticTask& task,
    std::ostream& os)
{
    dump_probabilistic_task_(task, os);
}

} // namespace probfd::task_properties
