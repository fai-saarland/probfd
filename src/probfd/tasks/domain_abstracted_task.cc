#include "probfd/tasks/domain_abstracted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#include <cassert>
#include <utility>

using namespace std;

using namespace downward;

namespace probfd::extra_tasks {

DomainAbstractedProbabilisticOperatorSpace::
    DomainAbstractedProbabilisticOperatorSpace(
        shared_ptr<ProbabilisticOperatorSpace> parent,
        std::shared_ptr<DomainAbstraction> domain_abstraction)
    : parent(std::move(parent))
    , domain_abstraction(std::move(domain_abstraction))
{
    if (task_properties::has_conditional_effects(*this->parent)) {
        ABORT(
            "DomainAbstractedProbabilisticOperatorSpace doesn't support "
            "conditional effects.");
    }
}

FactPair DomainAbstractedProbabilisticOperatorSpace::get_operator_precondition(
    int op_index,
    int fact_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_precondition(op_index, fact_index));
}

FactPair
DomainAbstractedProbabilisticOperatorSpace::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent
            ->get_operator_outcome_effect(op_index, outcome_index, eff_index));
}

int DomainAbstractedProbabilisticOperatorSpace::get_num_operators() const
{
    return parent->get_num_operators();
}

std::string
DomainAbstractedProbabilisticOperatorSpace::get_operator_name(int index) const
{
    return parent->get_operator_name(index);
}

int DomainAbstractedProbabilisticOperatorSpace::get_num_operator_preconditions(
    int index) const
{
    return parent->get_num_operator_preconditions(index);
}

int DomainAbstractedProbabilisticOperatorSpace::get_num_operator_outcomes(
    int op_index) const
{
    return parent->get_num_operator_outcomes(op_index);
}

value_t
DomainAbstractedProbabilisticOperatorSpace::get_operator_outcome_probability(
    int op_index,
    int outcome_index) const
{
    return parent->get_operator_outcome_probability(op_index, outcome_index);
}

int DomainAbstractedProbabilisticOperatorSpace::get_operator_outcome_id(
    int op_index,
    int outcome_index) const
{
    return parent->get_operator_outcome_id(op_index, outcome_index);
}

int DomainAbstractedProbabilisticOperatorSpace::
    get_num_operator_outcome_effects(int op_index, int outcome_index) const
{
    return parent->get_num_operator_outcome_effects(op_index, outcome_index);
}

int DomainAbstractedProbabilisticOperatorSpace::
    get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const
{
    return parent->get_num_operator_outcome_effect_conditions(
        op_index,
        outcome_index,
        eff_index);
}

FactPair DomainAbstractedProbabilisticOperatorSpace::
    get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_outcome_effect_condition(
            op_index,
            outcome_index,
            eff_index,
            cond_index));
}

} // namespace probfd::extra_tasks
