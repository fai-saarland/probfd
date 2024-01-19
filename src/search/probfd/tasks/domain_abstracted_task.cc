#include "probfd/tasks/domain_abstracted_task.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_proxy.h"

#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#include <cassert>
#include <utility>

using namespace std;

namespace probfd::extra_tasks {

DomainAbstractedTask::DomainAbstractedTask(
    const shared_ptr<ProbabilisticTask>& parent,
    vector<int>&& domain_size,
    vector<int>&& initial_state_values,
    vector<FactPair>&& goals,
    vector<vector<string>>&& fact_names,
    vector<vector<int>>&& value_map)
    : DelegatingTask(parent)
    , domain_size_(std::move(domain_size))
    , initial_state_values_(std::move(initial_state_values))
    , goals_(std::move(goals))
    , fact_names_(std::move(fact_names))
    , value_map_(std::move(value_map))
{
    if (parent->get_num_axioms() > 0) {
        ABORT("DomainAbstractedTask doesn't support axioms.");
    }
    if (task_properties::has_conditional_effects(
            ProbabilisticTaskProxy(*parent))) {
        ABORT("DomainAbstractedTask doesn't support conditional effects.");
    }
}

FactPair DomainAbstractedTask::get_abstract_fact(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.var, value_map_));
    assert(utils::in_bounds(fact.value, value_map_[fact.var]));
    return FactPair(fact.var, value_map_[fact.var][fact.value]);
}

int DomainAbstractedTask::get_variable_domain_size(int var) const
{
    return domain_size_[var];
}

string DomainAbstractedTask::get_fact_name(const FactPair& fact) const
{
    return fact_names_[fact.var][fact.value];
}

bool DomainAbstractedTask::are_facts_mutex(const FactPair&, const FactPair&)
    const
{
    ABORT("DomainAbstractedTask doesn't support querying mutexes.");
}

FactPair
DomainAbstractedTask::get_axiom_precondition(int op_index, int fact_index) const
{
    return get_abstract_fact(
        parent_->get_axiom_precondition(op_index, fact_index));
}

FactPair
DomainAbstractedTask::get_axiom_effect(int op_index, int eff_index) const
{
    return get_abstract_fact(parent_->get_axiom_effect(op_index, eff_index));
}

FactPair
DomainAbstractedTask::get_operator_precondition(int op_index, int fact_index)
    const
{
    return get_abstract_fact(
        parent_->get_operator_precondition(op_index, fact_index));
}

FactPair DomainAbstractedTask::get_operator_outcome_effect(
    int op_index,
    int outcome_index,
    int eff_index) const
{
    return get_abstract_fact(parent_->get_operator_outcome_effect(
        op_index,
        outcome_index,
        eff_index));
}

FactPair DomainAbstractedTask::get_goal_fact(int index) const
{
    return get_abstract_fact(parent_->get_goal_fact(index));
}

vector<int> DomainAbstractedTask::get_initial_state_values() const
{
    return initial_state_values_;
}

void DomainAbstractedTask::convert_state_values_from_parent(
    vector<int>& values) const
{
    int num_vars = domain_size_.size();
    for (int var = 0; var < num_vars; ++var) {
        int old_value = values[var];
        int new_value = value_map_[var][old_value];
        values[var] = new_value;
    }
}

} // namespace probfd::extra_tasks
