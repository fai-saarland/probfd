#include "pdb_abstracted_task.h"

#include "../../../globals.h"
#include "../../../global_operator.h"
#include "../../../utils/system.h"

#include <algorithm>
#include <numeric>
#include <cassert>

namespace tasks {

PDBAbstractedTask::PDBAbstractedTask(const pdbs::Pattern& pattern)
    : pattern(pattern)
{
    //assert(!task_properties::has_axioms(parent_proxy));
    assert(std::is_sorted(pattern.begin(), pattern.end()));

    // remember where in the pattern each variable is stored
    for (size_t i = 0; i < pattern.size(); ++i) {
        int var = pattern[i];
        var_to_index[var] = i;
    }

    // make a list of operator indices for operators which are
    // relevant to at least one variable in the pattern
    for (size_t opi = 0; opi < g_operators.size(); ++opi) {
        const GlobalOperator& op = g_operators[opi];

        std::vector<FactPair> effects;

        for (const GlobalEffect& eff : op.get_effects()) {
            FactPair effect = { eff.var, eff.val };
            int var = effect.first;

            if (var_to_index.find(var) != var_to_index.end()) {
                effects.push_back(convert_from_original_fact(effect));
            }
        }

        if (effects.empty()) {
            continue;
        }

        operator_indices.push_back(opi);
        operator_effects.push_back(effects);

        std::vector<FactPair> preconditions;

        for (const GlobalCondition& cond : op.get_preconditions()) {
            FactPair precondition = { cond.var, cond.val };
            int var = precondition.first;

            // check if the precondition variable appears in the pattern
            if (var_to_index.find(var) != var_to_index.end()) {
                preconditions.push_back(
                    convert_from_original_fact(precondition));
            }
        }

        std::sort(preconditions.begin(), preconditions.end());
        operator_preconditions.push_back(preconditions);
    }

    for (auto& [goal_var, goal_val] : g_goal) {
        FactPair goal = { goal_var, goal_val };

        auto it = std::find(pattern.begin(), pattern.end(), goal_var);
        if (it != std::end(pattern)) {
            goals.push_back(convert_from_original_fact(goal));
        }
    }

    assert(g_goal.size() >= goals.size());
}

int PDBAbstractedTask::get_original_variable_index(int index_in_pattern) const
{
    assert(index_in_pattern >= 0 &&
                   static_cast<unsigned>(index_in_pattern) < pattern.size());
    return pattern[index_in_pattern];
}

int PDBAbstractedTask::get_pattern_variable_index(int index_in_original) const
{
    auto it = var_to_index.find(index_in_original);
    if (it != var_to_index.end()) {
        return it->second;
    } else {
        std::cout << "PDBAbstractedTask: "
                  << "A function tried to access a variable that is not part"
                  << "of the pattern."
                  << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }
}

FactPair
PDBAbstractedTask::convert_from_pattern_fact(const FactPair &fact) const
{
    return {get_original_variable_index(fact.first), fact.second};
}

FactPair
PDBAbstractedTask::convert_from_original_fact(const FactPair &fact) const
{
    return {get_pattern_variable_index(fact.first), fact.second};
}

int PDBAbstractedTask::get_num_variables() const
{
    return pattern.size();
}

std::string PDBAbstractedTask::get_variable_name(int var) const
{
    var = get_original_variable_index(var);
    return g_variable_name[var];
}

int PDBAbstractedTask::get_variable_domain_size(int var) const
{
    var = get_original_variable_index(var);
    return g_variable_domain[var];
}

int PDBAbstractedTask::get_variable_axiom_layer(int var) const
{
    var = get_original_variable_index(var);
    return g_axiom_layers[var];
}

int PDBAbstractedTask::get_variable_default_axiom_value(int var) const
{
    var = get_original_variable_index(var);
    return g_default_axiom_values[var];
}

std::string PDBAbstractedTask::get_fact_name(const FactPair &fact) const
{
    int var = get_original_variable_index(fact.first);
    return g_fact_names[var][fact.second];
}

int PDBAbstractedTask::get_operator_cost(int index) const
{
    index = convert_operator_index_to_parent(index);
    return g_operators[index].get_cost();
}

std::string
PDBAbstractedTask::get_operator_name(int index) const
{
    index = convert_operator_index_to_parent(index);
    return g_operators[index].get_name();
}

int PDBAbstractedTask::get_num_operators() const
{
    return operator_indices.size();
}

int PDBAbstractedTask::get_num_operator_preconditions(int index) const
{
    return operator_preconditions[index].size();
}

FactPair
PDBAbstractedTask::
get_operator_precondition(int op_index, int fact_index) const
{
    return operator_preconditions[op_index][fact_index];
}

int PDBAbstractedTask::get_num_operator_effects(int op_index) const
{
    return operator_effects[op_index].size();
}

FactPair
PDBAbstractedTask::get_operator_effect(int op_index, int eff_index) const
{
    return operator_effects[op_index][eff_index];
}

int PDBAbstractedTask::convert_operator_index_to_parent(int index) const
{
    assert(
        index >= 0 && static_cast<unsigned>(index) < operator_indices.size());
    return operator_indices[index];
}

int PDBAbstractedTask::get_num_goals() const
{
    return goals.size();
}

FactPair PDBAbstractedTask::get_goal_fact(int index) const
{
    return goals[index];
}

std::vector<int> PDBAbstractedTask::get_initial_state_values() const
{
    std::vector<int> values = g_initial_state_data;
    convert_parent_state_values(values);
    return values;
}

std::vector<int>
PDBAbstractedTask::convert_parent_state_values(const GlobalState &state) const
{
    std::vector<int> converted;

    for(int index : pattern) {
        converted.push_back(state[index]);
    }

    return converted;
}

void
PDBAbstractedTask::
convert_parent_state_values(std::vector<int> &values) const
{
    std::vector<int> converted;

    for(int index : pattern) {
        converted.push_back(values[index]);
    }

    values.swap(converted);
}

bool PDBAbstractedTask::is_goal_state(const std::vector<int>& state) const
{
    for (FactPair goal : goals) {
        if (state[goal.first] != goal.second)
            return false;
    }

    return true;
}

std::vector<int> PDBAbstractedTask::get_domain_values() const
{
    std::vector<int> domains(pattern.size());

    for (size_t i = 0; i < pattern.size(); ++i) {
        domains[i] = g_variable_domain[pattern[i]];
    }

    return domains;
}

std::unique_ptr<successor_generator::SuccessorGenerator<int>>
PDBAbstractedTask::get_successor_generator()
{
    std::vector<int> op_indices(operator_preconditions.size());
    std::iota(op_indices.begin(), op_indices.end(), 0);

    return std::make_unique<successor_generator::SuccessorGenerator<int>>(
        get_domain_values(),
        operator_preconditions,
        op_indices);
}

}
