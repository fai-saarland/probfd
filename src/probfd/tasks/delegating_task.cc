#include "probfd/tasks/delegating_task.h"

using namespace std;

using namespace downward;

namespace probfd::tasks {

DelegatingTask::DelegatingTask(const shared_ptr<ProbabilisticTask>& parent)
    : parent_(parent)
{
}

int DelegatingTask::get_num_variables() const
{
    return parent_->get_num_variables();
}

string DelegatingTask::get_variable_name(int var) const
{
    return parent_->get_variable_name(var);
}

int DelegatingTask::get_variable_domain_size(int var) const
{
    return parent_->get_variable_domain_size(var);
}

int DelegatingTask::get_variable_axiom_layer(int var) const
{
    return parent_->get_variable_axiom_layer(var);
}

int DelegatingTask::get_variable_default_axiom_value(int var) const
{
    return parent_->get_variable_default_axiom_value(var);
}

string DelegatingTask::get_fact_name(const FactPair& fact) const
{
    return parent_->get_fact_name(fact);
}

int DelegatingTask::get_num_axioms() const
{
    return parent_->get_num_axioms();
}

string DelegatingTask::get_axiom_name(int index) const
{
    return parent_->get_axiom_name(index);
}

int DelegatingTask::get_num_axiom_preconditions(int index) const
{
    return parent_->get_num_axiom_preconditions(index);
}

FactPair
DelegatingTask::get_axiom_precondition(int op_index, int fact_index) const
{
    return parent_->get_axiom_precondition(op_index, fact_index);
}

int DelegatingTask::get_num_axiom_effects(int op_index) const
{
    return parent_->get_num_axiom_effects(op_index);
}

int DelegatingTask::get_num_axiom_effect_conditions(int op_index, int eff_index)
    const
{
    return parent_->get_num_axiom_effect_conditions(op_index, eff_index);
}

FactPair DelegatingTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return parent_->get_axiom_effect_condition(op_index, eff_index, cond_index);
}

FactPair DelegatingTask::get_axiom_effect(int op_index, int eff_index) const
{
    return parent_->get_axiom_effect(op_index, eff_index);
}

value_t DelegatingTask::get_operator_cost(int index) const
{
    return parent_->get_operator_cost(index);
}

string DelegatingTask::get_operator_name(int index) const
{
    return parent_->get_operator_name(index);
}

int DelegatingTask::get_num_operators() const
{
    return parent_->get_num_operators();
}

int DelegatingTask::get_num_operator_preconditions(int index) const
{
    return parent_->get_num_operator_preconditions(index);
}

FactPair
DelegatingTask::get_operator_precondition(int op_index, int fact_index) const
{
    return parent_->get_operator_precondition(op_index, fact_index);
}

int DelegatingTask::get_num_goals() const
{
    return parent_->get_num_goals();
}

FactPair DelegatingTask::get_goal_fact(int index) const
{
    return parent_->get_goal_fact(index);
}

vector<int> DelegatingTask::get_initial_state_values() const
{
    return parent_->get_initial_state_values();
}

int DelegatingTask::get_num_operator_outcomes(int op_index) const
{
    return parent_->get_num_operator_outcomes(op_index);
}

value_t DelegatingTask::get_operator_outcome_probability(
    int op_index,
    int outcome_index) const
{
    return parent_->get_operator_outcome_probability(op_index, outcome_index);
}

int DelegatingTask::get_operator_outcome_id(int op_index, int outcome_index)
    const
{
    return parent_->get_operator_outcome_id(op_index, outcome_index);
}

int DelegatingTask::get_num_operator_outcome_effects(
    int op_index,
    int outcome_index) const
{
    return parent_->get_num_operator_outcome_effects(op_index, outcome_index);
}

FactPair DelegatingTask::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return parent_->get_operator_outcome_effect(
        op_index,
        outcome_index,
        eff_index);
}

int DelegatingTask::get_num_operator_outcome_effect_conditions(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return parent_->get_num_operator_outcome_effect_conditions(
        op_index,
        outcome_index,
        eff_index);
}

FactPair DelegatingTask::get_operator_outcome_effect_condition(
    int op_index,
    int outcome_index,
    int eff_index,
    int cond_index) const
{
    return parent_->get_operator_outcome_effect_condition(
        op_index,
        outcome_index,
        eff_index,
        cond_index);
}

value_t DelegatingTask::get_goal_termination_cost() const
{
    return parent_->get_non_goal_termination_cost();
}

value_t DelegatingTask::get_non_goal_termination_cost() const
{
    return parent_->get_non_goal_termination_cost();
}

} // namespace probfd::tasks
