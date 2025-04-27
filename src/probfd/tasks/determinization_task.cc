#include "probfd/tasks/determinization_task.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"

using namespace downward;

namespace probfd::tasks {

DeterminizationOperatorMapping::DeterminizationOperatorMapping(
    const ProbabilisticOperatorSpace& parent_operators)
{
    for (ProbabilisticOperatorProxy op_proxy : parent_operators) {
        const int num_outcomes = op_proxy.get_outcomes().size();
        for (int j = 0; j != num_outcomes; ++j) {
            det_to_prob_index_.emplace_back(op_proxy.get_id(), j);
        }
    }
}

std::pair<int, int> DeterminizationOperatorMapping::get_parent_indices(
    int deterministic_operator_index) const
{
    return det_to_prob_index_[deterministic_operator_index];
}

std::size_t DeterminizationOperatorMapping::num_operators() const
{
    return det_to_prob_index_.size();
}

DeterminizationOperatorSpace::DeterminizationOperatorSpace(
    std::shared_ptr<ProbabilisticOperatorSpace> operators,
    std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index)
    : parent_operators_(std::move(operators))
    , det_to_prob_index_(std::move(det_to_prob_index))
{
}

std::string DeterminizationOperatorSpace::get_operator_name(int index) const
{
    return parent_operators_->get_operator_name(
        det_to_prob_index_->get_parent_indices(index).first);
}

int DeterminizationOperatorSpace::get_num_operators() const
{
    return det_to_prob_index_->num_operators();
}

int DeterminizationOperatorSpace::get_num_operator_preconditions(
    int index) const
{
    return parent_operators_->get_num_operator_preconditions(
        det_to_prob_index_->get_parent_indices(index).first);
}

FactPair DeterminizationOperatorSpace::get_operator_precondition(
    int op_index,
    int fact_index) const
{
    return parent_operators_->get_operator_precondition(
        det_to_prob_index_->get_parent_indices(op_index).first,
        fact_index);
}

int DeterminizationOperatorSpace::get_num_operator_effects(int op_index) const
{
    const auto& [pr_op_index, pr_outcome_index] =
        det_to_prob_index_->get_parent_indices(op_index);
    return parent_operators_->get_num_operator_outcome_effects(
        pr_op_index,
        pr_outcome_index);
}

int DeterminizationOperatorSpace::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    const auto& [pr_op_index, pr_outcome_index] =
        det_to_prob_index_->get_parent_indices(op_index);
    return parent_operators_->get_num_operator_outcome_effect_conditions(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

FactPair DeterminizationOperatorSpace::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    const auto& [pr_op_index, pr_outcome_index] =
        det_to_prob_index_->get_parent_indices(op_index);
    return parent_operators_->get_operator_outcome_effect_condition(
        pr_op_index,
        pr_outcome_index,
        eff_index,
        cond_index);
}

FactPair
DeterminizationOperatorSpace::get_operator_effect(int op_index, int eff_index)
    const
{
    const auto& [pr_op_index, pr_outcome_index] =
        det_to_prob_index_->get_parent_indices(op_index);
    return parent_operators_->get_operator_outcome_effect(
        pr_op_index,
        pr_outcome_index,
        eff_index);
}

DeterminizationCostFunction::DeterminizationCostFunction(
    std::shared_ptr<OperatorCostFunction<value_t>> parent_cost_function,
    std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index)
    : parent_cost_function_(std::move(parent_cost_function))
    , det_to_prob_index_(std::move(det_to_prob_index))
{
}

int DeterminizationCostFunction::get_operator_cost(int index) const
{
    return static_cast<int>(parent_cost_function_->get_operator_cost(
        det_to_prob_index_->get_parent_indices(index).first));
}

extern SharedAbstractTask
create_determinization_task(SharedProbabilisticTask probabilistic_task)
{
    auto operator_mapping = std::make_shared<DeterminizationOperatorMapping>(
        get_operators(probabilistic_task));

    return std::forward_as_tuple(
        get_shared_variables(probabilistic_task),
        get_shared_axioms(probabilistic_task),
        make_shared<DeterminizationOperatorSpace>(
            get_shared_operators(probabilistic_task),
            operator_mapping),
        get_shared_goal(probabilistic_task),
        get_shared_init(probabilistic_task),
        make_shared<DeterminizationCostFunction>(
            get_shared_cost_function(probabilistic_task),
            operator_mapping));
}

} // namespace probfd::tasks
