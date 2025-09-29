#include "downward/task_utils/task_properties.h"

#include "downward/utils/logging.h"
#include "downward/utils/memory.h"
#include "downward/utils/system.h"

#include "downward/axiom_space.h"
#include "downward/initial_state_values.h"
#include "downward/operator_cost_function.h"

#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;
using downward::utils::ExitCode;

namespace downward::task_properties {

bool is_unit_cost(
    const OperatorSpace& operators,
    const OperatorIntCostFunction& cost_function)
{
    for (const auto op : operators) {
        if (cost_function.get_operator_cost(op.get_id()) != 1) return false;
    }
    return true;
}

bool has_axioms(const AxiomSpace& axiom_space)
{
    return axiom_space.get_num_axioms() != 0;
}

void verify_no_axioms(const AxiomSpace& axiom_space)
{
    if (has_axioms(axiom_space)) {
        throw utils::UnsupportedError(
            "This configuration does not support axioms!");
    }
}

static int
get_first_conditional_effects_op_id(const ClassicalOperatorSpace& operators)
{
    for (OperatorProxy op : operators) {
        for (auto effect : op.get_effects()) {
            if (!effect.get_conditions().empty()) return op.get_id();
        }
    }
    return -1;
}

bool has_conditional_effects(const ClassicalOperatorSpace& operators)
{
    return get_first_conditional_effects_op_id(operators) != -1;
}

void verify_no_conditional_effects(const ClassicalOperatorSpace& operators)
{
    if (const int op_id = get_first_conditional_effects_op_id(operators);
        op_id != -1) {
        throw utils::UnsupportedError(
            "This configuration does not support conditional effects "
            "(operator {})!",
            operators[op_id].get_name());
    }
}

vector<int> get_operator_costs(
    const OperatorSpace& operators,
    const OperatorIntCostFunction& cost_function)
{
    vector<int> costs;
    costs.reserve(operators.size());
    for (const auto op : operators)
        costs.push_back(cost_function.get_operator_cost(op.get_id()));
    return costs;
}

double get_average_operator_cost(
    const OperatorSpace& operators,
    const OperatorIntCostFunction& cost_function)
{
    double average_operator_cost = 0;
    for (const auto op : operators) {
        average_operator_cost += cost_function.get_operator_cost(op.get_id());
    }
    average_operator_cost /= operators.size();
    return average_operator_cost;
}

int get_min_operator_cost(
    const OperatorSpace& operators,
    const OperatorIntCostFunction& cost_function)
{
    int min_cost = numeric_limits<int>::max();
    for (const auto op : operators) {
        min_cost = min(min_cost, cost_function.get_operator_cost(op.get_id()));
    }
    return min_cost;
}

int get_num_facts(const VariableSpace& variables)
{
    int num_facts = 0;
    for (VariableProxy var : variables) num_facts += var.get_domain_size();
    return num_facts;
}

int get_num_total_effects(
    const AxiomSpace& axioms,
    const ClassicalOperatorSpace& operators)
{
    int num_effects = 0;
    for (OperatorProxy op : operators) num_effects += op.get_effects().size();
    num_effects += axioms.size();
    return num_effects;
}

void print_variable_statistics(
    const VariableSpace& variables,
    const int_packer::IntPacker& state_packer)
{
    int num_facts = 0;
    for (VariableProxy var : variables) num_facts += var.get_domain_size();

    utils::g_log << "Variables: " << variables.size() << endl;
    utils::g_log << "FactPairs: " << num_facts << endl;
    utils::g_log << "Bytes per state: "
                 << state_packer.get_num_bins() *
                        sizeof(int_packer::IntPacker::Bin)
                 << endl;
}

void dump_pddl(const VariableSpace& variable_space, const State& state)
{
    for (FactPair fact : state | as_fact_pair_set) {
        string fact_name = variable_space.get_fact_proxy(fact).get_name();
        if (fact_name != "<none of those>") utils::g_log << fact_name << endl;
    }
}

void dump_fdr(const VariableSpace& variables, const State& state)
{
    for (VariableProxy var : variables) {
        utils::g_log << "  #" << var.get_id() << " [" << var.get_name()
                     << "] -> " << state[var] << endl;
    }
}

void dump_goals(const VariableSpace& variables, const GoalFactList& goals)
{
    utils::g_log << "Goal conditions:" << endl;
    for (const auto [var_id, value] : goals) {
        auto var = variables[var_id];
        utils::g_log << "  " << var.get_name() << ": " << value << endl;
    }
}

void dump_task(const AbstractTaskTuple& task)
{
    const auto& [variables, axioms, operators, goals, init_values, cost_function] =
        task;

    int min_action_cost = numeric_limits<int>::max();
    int max_action_cost = 0;

    for (auto op : operators) {
        min_action_cost =
            min(min_action_cost, cost_function.get_operator_cost(op.get_id()));
        max_action_cost =
            max(max_action_cost, cost_function.get_operator_cost(op.get_id()));
    }

    utils::g_log << "Min action cost: " << min_action_cost << endl;
    utils::g_log << "Max action cost: " << max_action_cost << endl;

    utils::g_log << "Variables (" << variables.size() << "):" << endl;
    for (VariableProxy var : variables) {
        utils::g_log << "  " << var.get_name() << " (range "
                     << var.get_domain_size() << ")" << endl;
        for (int val = 0; val < var.get_domain_size(); ++val) {
            utils::g_log << "    " << val << ": "
                         << var.get_fact(val).get_name() << endl;
        }
    }

    State initial_state = init_values.get_initial_state();
    utils::g_log << "Initial state (PDDL):" << endl;
    dump_pddl(variables, initial_state);
    utils::g_log << "Initial state (FDR):" << endl;
    dump_fdr(variables, initial_state);
    dump_goals(variables, goals);
}

PerComponentInformation<int_packer::IntPacker, VariableSpace>
    g_state_packers([](const VariableSpace& variables) {
        vector<int> variable_ranges;
        variable_ranges.reserve(variables.size());
        for (VariableProxy var : variables) {
            variable_ranges.push_back(var.get_domain_size());
        }
        return std::make_unique<int_packer::IntPacker>(variable_ranges);
    });
} // namespace downward::task_properties
