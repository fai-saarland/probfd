#include "downward/tasks/delegating_task.h"

using namespace std;

namespace downward::tasks {
DelegatingTask::DelegatingTask(const shared_ptr<AbstractTask>& parent)
    : parent(parent)
{
}

int DelegatingTask::get_num_variables() const
{
    return parent->get_num_variables();
}

string DelegatingTask::get_variable_name(int var) const
{
    return parent->get_variable_name(var);
}

int DelegatingTask::get_variable_domain_size(int var) const
{
    return parent->get_variable_domain_size(var);
}

int DelegatingTask::get_variable_axiom_layer(int var) const
{
    return parent->get_variable_axiom_layer(var);
}

int DelegatingTask::get_variable_default_axiom_value(int var) const
{
    return parent->get_variable_default_axiom_value(var);
}

string DelegatingTask::get_fact_name(const FactPair& fact) const
{
    return parent->get_fact_name(fact);
}

int DelegatingTask::get_num_axioms() const
{
    return parent->get_num_axioms();
}

string DelegatingTask::get_axiom_name(int index) const
{
    return parent->get_axiom_name(index);
}

int DelegatingTask::get_num_axiom_preconditions(int index) const
{
    return parent->get_num_axiom_preconditions(index);
}

FactPair
DelegatingTask::get_axiom_precondition(int op_index, int fact_index) const
{
    return parent->get_axiom_precondition(op_index, fact_index);
}

int DelegatingTask::get_num_axiom_effects(int op_index) const
{
    return parent->get_num_axiom_effects(op_index);
}

int DelegatingTask::get_num_axiom_effect_conditions(int op_index, int eff_index)
    const
{
    return parent->get_num_axiom_effect_conditions(op_index, eff_index);
}

FactPair DelegatingTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return parent->get_axiom_effect_condition(op_index, eff_index, cond_index);
}

FactPair DelegatingTask::get_axiom_effect(int op_index, int eff_index) const
{
    return parent->get_axiom_effect(op_index, eff_index);
}

int DelegatingTask::get_operator_cost(int index) const
{
    return parent->get_operator_cost(index);
}

string DelegatingTask::get_operator_name(int index) const
{
    return parent->get_operator_name(index);
}

int DelegatingTask::get_num_operators() const
{
    return parent->get_num_operators();
}

int DelegatingTask::get_num_operator_preconditions(int index) const
{
    return parent->get_num_operator_preconditions(index);
}

FactPair
DelegatingTask::get_operator_precondition(int op_index, int fact_index) const
{
    return parent->get_operator_precondition(op_index, fact_index);
}

int DelegatingTask::get_num_operator_effects(int op_index) const
{
    return parent->get_num_operator_effects(op_index);
}

int DelegatingTask::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    return parent->get_num_operator_effect_conditions(op_index, eff_index);
}

FactPair DelegatingTask::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return parent->get_operator_effect_condition(
        op_index,
        eff_index,
        cond_index);
}

FactPair DelegatingTask::get_operator_effect(int op_index, int eff_index) const
{
    return parent->get_operator_effect(op_index, eff_index);
}

int DelegatingTask::convert_operator_index(
    int index,
    const PlanningTask* ancestor_task) const
{
    if (ancestor_task == this) {
        return index;
    }
    int parent_index = convert_operator_index_to_parent(index);
    return parent->convert_operator_index(parent_index, ancestor_task);
}

int DelegatingTask::get_num_goals() const
{
    return parent->get_num_goals();
}

FactPair DelegatingTask::get_goal_fact(int index) const
{
    return parent->get_goal_fact(index);
}

vector<int> DelegatingTask::get_initial_state_values() const
{
    return parent->get_initial_state_values();
}

void DelegatingTask::convert_ancestor_state_values(
    vector<int>& values,
    const PlanningTask* ancestor_task) const
{
    if (this == ancestor_task) {
        return;
    }
    parent->convert_ancestor_state_values(values, ancestor_task);
    convert_state_values_from_parent(values);
}
} // namespace tasks
