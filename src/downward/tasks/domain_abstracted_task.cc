#include "downward/tasks/domain_abstracted_task.h"

#include "downward/transformations/domain_abstraction.h"

#include "downward/utils/system.h"

using namespace std;

namespace downward::extra_tasks {
/*
  If we need the same functionality again in another task, we can move this to
  actract_task.h. We should then document that this method is only supposed to
  be used from within AbstractTasks.
*/
static bool has_conditional_effects(const AbstractTask& task)
{
    int num_ops = task.get_num_operators();
    for (int op_index = 0; op_index < num_ops; ++op_index) {
        int num_effs = task.get_num_operator_effects(op_index);
        for (int eff_index = 0; eff_index < num_effs; ++eff_index) {
            int num_conditions =
                task.get_num_operator_effect_conditions(op_index, eff_index);
            if (num_conditions > 0) {
                return true;
            }
        }
    }
    return false;
}

DomainAbstractedTask::DomainAbstractedTask(
    const shared_ptr<AbstractTask>& parent,
    vector<int>&& domain_size,
    vector<int>&& initial_state_values,
    vector<FactPair>&& goals,
    vector<vector<string>>&& fact_names,
    std::shared_ptr<DomainAbstraction> domain_abstraction)
    : DelegatingTask(parent)
    , domain_size(std::move(domain_size))
    , initial_state_values(std::move(initial_state_values))
    , goals(std::move(goals))
    , fact_names(std::move(fact_names))
    , domain_abstraction(std::move(domain_abstraction))
{
    if (parent->get_num_axioms() > 0) {
        ABORT("DomainAbstractedTask doesn't support axioms.");
    }
    if (has_conditional_effects(*parent)) {
        ABORT("DomainAbstractedTask doesn't support conditional effects.");
    }
}

int DomainAbstractedTask::get_variable_domain_size(int var) const
{
    return domain_size[var];
}

string DomainAbstractedTask::get_fact_name(const FactPair& fact) const
{
    return fact_names[fact.var][fact.value];
}

FactPair
DomainAbstractedTask::get_axiom_precondition(int op_index, int fact_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_axiom_precondition(op_index, fact_index));
}

FactPair
DomainAbstractedTask::get_axiom_effect(int op_index, int eff_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_axiom_effect(op_index, eff_index));
}

FactPair
DomainAbstractedTask::get_operator_precondition(int op_index, int fact_index)
    const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_precondition(op_index, fact_index));
}

FactPair
DomainAbstractedTask::get_operator_effect(int op_index, int eff_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_effect(op_index, eff_index));
}

FactPair DomainAbstractedTask::get_goal_fact(int index) const
{
    return domain_abstraction->get_abstract_fact(parent->get_goal_fact(index));
}

vector<int> DomainAbstractedTask::get_initial_state_values() const
{
    return initial_state_values;
}

} // namespace extra_tasks
