#include "probfd/tasks/all_outcomes_determinization.h"

namespace probfd {
namespace tasks {

AODDeterminizationTask::AODDeterminizationTask(
    const ProbabilisticTask* parent_task)
    : parent_task(parent_task)
{
    ProbabilisticTaskProxy proxy(*parent_task);

    for (ProbabilisticOperatorProxy op_proxy : proxy.get_operators()) {
        const int num_outcomes = op_proxy.get_outcomes().size();
        for (int j = 0; j != num_outcomes; ++j) {
            det_to_prob_index.emplace_back(op_proxy.get_id(), j);
        }
    }
}

int AODDeterminizationTask::get_num_variables() const
{
    return parent_task->get_num_variables();
}

std::string AODDeterminizationTask::get_variable_name(int var) const
{
    return parent_task->get_variable_name(var);
}

int AODDeterminizationTask::get_variable_domain_size(int var) const
{
    return parent_task->get_variable_domain_size(var);
}

int AODDeterminizationTask::get_variable_axiom_layer(int var) const
{
    return parent_task->get_variable_axiom_layer(var);
}

int AODDeterminizationTask::get_variable_default_axiom_value(int var) const
{
    return parent_task->get_variable_default_axiom_value(var);
}

std::string AODDeterminizationTask::get_fact_name(const FactPair& fact) const
{
    return parent_task->get_fact_name(fact);
}

bool AODDeterminizationTask::are_facts_mutex(
    const FactPair& fact1,
    const FactPair& fact2) const
{
    return parent_task->are_facts_mutex(fact1, fact2);
}

int AODDeterminizationTask::get_num_axioms() const
{
    return parent_task->get_num_axioms();
}

std::string AODDeterminizationTask::get_axiom_name(int index) const
{
    return parent_task->get_axiom_name(index);
}

int AODDeterminizationTask::get_num_axiom_preconditions(int index) const
{
    return parent_task->get_num_axiom_preconditions(index);
}

FactPair
AODDeterminizationTask::get_axiom_precondition(int op_index, int fact_index)
    const
{
    return parent_task->get_axiom_precondition(op_index, fact_index);
}

int AODDeterminizationTask::get_num_axiom_effects(int op_index) const
{
    return parent_task->get_num_axiom_effects(op_index);
}

int AODDeterminizationTask::get_num_axiom_effect_conditions(
    int op_index,
    int eff_index) const
{
    return parent_task->get_num_axiom_effect_conditions(op_index, eff_index);
}

FactPair AODDeterminizationTask::get_axiom_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return parent_task->get_axiom_effect_condition(
        op_index,
        eff_index,
        cond_index);
}

FactPair
AODDeterminizationTask::get_axiom_effect(int op_index, int eff_index) const
{
    return parent_task->get_axiom_effect(op_index, eff_index);
}

int AODDeterminizationTask::get_operator_cost(int index) const
{
    return parent_task->get_operator_cost(get_parent_indices(index).first);
}

std::string AODDeterminizationTask::get_operator_name(int index) const
{
    return parent_task->get_operator_name(get_parent_indices(index).first);
}

int AODDeterminizationTask::get_num_operators() const
{
    return det_to_prob_index.size();
}

int AODDeterminizationTask::get_num_operator_preconditions(int index) const
{
    return parent_task->get_num_operator_preconditions(
        get_parent_indices(index).first);
}

FactPair
AODDeterminizationTask::get_operator_precondition(int op_index, int fact_index)
    const
{
    return parent_task->get_operator_precondition(
        get_parent_indices(op_index).first,
        fact_index);
}

int AODDeterminizationTask::get_num_operator_effects(int op_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task->get_num_operator_outcome_effects(
        pr_op_index,
        pr_outcome_index);
}

int AODDeterminizationTask::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task->get_num_operator_outcome_effect_conditions(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

FactPair AODDeterminizationTask::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task->get_operator_outcome_effect_condition(
        pr_op_index,
        pr_outcome_index,
        eff_index,
        cond_index);
}

FactPair
AODDeterminizationTask::get_operator_effect(int op_index, int eff_index) const
{
    const auto& [pr_op_index, pr_outcome_index] = get_parent_indices(op_index);
    return parent_task->get_operator_outcome_effect(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

int AODDeterminizationTask::get_num_goals() const
{
    return parent_task->get_num_goals();
}

FactPair AODDeterminizationTask::get_goal_fact(int index) const
{
    return parent_task->get_goal_fact(index);
}

std::vector<int> AODDeterminizationTask::get_initial_state_values() const
{
    return parent_task->get_initial_state_values();
}

/*
 * Convert state values from an ancestor task A (ancestor) into
 * state values from this task, C (child). Task A has to be an
 * ancestor of C in the sense that C is the result of a sequence of
 * task transformations on A.
 * The values are converted in-place to avoid unnecessary copies. If a
 * subclass needs to create a new vector, e.g., because the size changes,
 * it should create the new vector in a local variable and then swap it with
 * the parameter.
 */
void AODDeterminizationTask::convert_ancestor_state_values(
    std::vector<int>&,
    const AbstractTaskBase* ancestor_task) const
{
    (void)ancestor_task;
    assert(ancestor_task == this || ancestor_task == parent_task);
}
/*
 * Convert an operator index from this task, C (child), into an operator
 * index from an ancestor task A (ancestor). Task A has to be an ancestor of
 * C in the sense that C is the result of a sequence of task transformations
 * on A.
 */
int AODDeterminizationTask::convert_operator_index(
    int index,
    const AbstractTaskBase* ancestor_task) const
{
    (void)ancestor_task;
    assert(ancestor_task == this);
    return index;
}

std::pair<int, int> AODDeterminizationTask::get_parent_indices(
    int deterministic_operator_index) const
{
    return det_to_prob_index[deterministic_operator_index];
}

} // namespace tasks
} // namespace probfd