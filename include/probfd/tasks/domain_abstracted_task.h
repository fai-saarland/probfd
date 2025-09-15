#ifndef PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H
#define PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H

#include "downward/transformations/domain_abstraction.h"

#include "probfd/probabilistic_operator_space.h"

#include <memory>
#include <string>
#include <vector>

namespace probfd::extra_tasks {

class DomainAbstractedProbabilisticOperatorSpace
    : public ProbabilisticOperatorSpace {
    const std::shared_ptr<ProbabilisticOperatorSpace> parent;
    const std::shared_ptr<downward::DomainAbstraction> domain_abstraction;

public:
    DomainAbstractedProbabilisticOperatorSpace(
        std::shared_ptr<ProbabilisticOperatorSpace> parent,
        std::shared_ptr<downward::DomainAbstraction> domain_abstraction);

    int get_num_operators() const override;

    std::string get_operator_name(int index) const override;

    int get_num_operator_preconditions(int index) const override;
    downward::FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    int get_num_operator_outcomes(int op_index) const override;

    value_t get_operator_outcome_probability(int op_index, int outcome_index)
        const override;

    int get_operator_outcome_id(int op_index, int outcome_index) const override;

    int get_num_operator_outcome_effects(int op_index, int outcome_index)
        const override;

    int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const override;

    downward::FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const override;

    downward::FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const override;
};

} // namespace probfd::extra_tasks

#endif
