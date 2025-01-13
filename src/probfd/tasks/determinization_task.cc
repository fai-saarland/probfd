#include "probfd/tasks/determinization_task.h"

#include "probfd/probabilistic_task.h"

#include "probfd/task_proxy.h"

#include <cassert>

namespace probfd::tasks {

DeterminizationTask::DeterminizationTask(
    std::shared_ptr<ProbabilisticTask> parent_task)
    : parent_task_(std::move(parent_task))
{
    ProbabilisticTaskProxy proxy(*parent_task_);

    for (ProbabilisticOperatorProxy op_proxy : proxy.get_operators()) {
        const int num_outcomes = op_proxy.get_outcomes().size();
        for (int j = 0; j != num_outcomes; ++j) {
            det_to_prob_index_.emplace_back(op_proxy.get_id(), j);
        }
    }
}

int DeterminizationTask::get_num_variables() const
{
    return parent_task_->get_num_variables();
}

std::string DeterminizationTask::get_variable_name(int var) const
{
    return parent_task_->get_variable_name(var);
}

int DeterminizationTask::get_variable_domain_size(int var) const
{
    return parent_task_->get_variable_domain_size(var);
}

int DeterminizationTask::get_variable_axiom_layer(int var) const
{
    return parent_task_->get_variable_axiom_layer(var);
}

int DeterminizationTask::get_variable_default_axiom_value(int var) const
{
    return parent_task_->get_variable_default_axiom_value(var);
}

std::string DeterminizationTask::get_fact_name(const FactPair& fact) const
{
    return parent_task_->get_fact_name(fact);
}

bool DeterminizationTask::are_facts_mutex(
    const FactPair& fact1,
    const FactPair& fact2) const
{
    return parent_task_->are_facts_mutex(fact1, fact2);
}

int DeterminizationTask::get_num_axioms() const
{
    return parent_task_->get_num_axioms();
}

std::string DeterminizationTask::get_axiom_name(int index) const
{
    return parent_task_->get_axiom_name(index);
}

int DeterminizationTask::get_num_axiom_preconditions(int index) const
{
    return parent_task_->get_num_axiom_preconditions(index);
}

FactPair
DeterminizationTask::get_axiom_precondition(int op_index, int fact_index) const
{
    return parent_task_->get_axiom_precondition(op_index, fact_index);
}

int DeterminizationTask::get_num_axiom_effects(int op_index) const
{
    return parent_task_->get_num_axiom_effects(op_index);
}

int DeterminizationTask::get_num_axiom_effect_conditions(
    int op_index,
    int eff_index) const
{
    return parent_task_->get_num_axiom_effect_conditions(op_index, eff_index);
}

FactPair DeterminizationTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return parent_task_->get_axiom_effect_condition(
        op_index,
        eff_index,
        cond_index);
}

FactPair
DeterminizationTask::get_axiom_effect(int op_index, int eff_index) const
{
    return parent_task_->get_axiom_effect(op_index, eff_index);
}

int DeterminizationTask::get_operator_cost(int index) const
{
    return static_cast<int>(
        parent_task_->get_operator_cost(get_parent_indices(index).first));
}

std::string DeterminizationTask::get_operator_name(int index) const
{
    return parent_task_->get_operator_name(get_parent_indices(index).first);
}

int DeterminizationTask::get_num_operators() const
{
    return det_to_prob_index_.size();
}

int DeterminizationTask::get_num_operator_preconditions(int index) const
{
    return parent_task_->get_num_operator_preconditions(
        get_parent_indices(index).first);
}

FactPair
DeterminizationTask::get_operator_precondition(int op_index, int fact_index)
    const
{
    return parent_task_->get_operator_precondition(
        get_parent_indices(op_index).first,
        fact_index);
}

int DeterminizationTask::get_num_operator_effects(int op_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task_->get_num_operator_outcome_effects(
        pr_op_index,
        pr_outcome_index);
}

int DeterminizationTask::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task_->get_num_operator_outcome_effect_conditions(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

FactPair DeterminizationTask::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task_->get_operator_outcome_effect_condition(
        pr_op_index,
        pr_outcome_index,
        eff_index,
        cond_index);
}

FactPair
DeterminizationTask::get_operator_effect(int op_index, int eff_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task_->get_operator_outcome_effect(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

int DeterminizationTask::get_num_goals() const
{
    return parent_task_->get_num_goals();
}

FactPair DeterminizationTask::get_goal_fact(int index) const
{
    return parent_task_->get_goal_fact(index);
}

std::vector<int> DeterminizationTask::get_initial_state_values() const
{
    return parent_task_->get_initial_state_values();
}

std::pair<int, int>
DeterminizationTask::get_parent_indices(int deterministic_operator_index) const
{
    return det_to_prob_index_[deterministic_operator_index];
}

} // namespace probfd::tasks
