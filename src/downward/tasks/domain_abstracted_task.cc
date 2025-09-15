#include "downward/tasks/domain_abstracted_task.h"

#include "downward/task_utils/task_properties.h"
#include "downward/transformations/domain_abstraction.h"

#include "downward/utils/system.h"

using namespace std;

namespace downward::extra_tasks {

DomainAbstractedVariableSpace::DomainAbstractedVariableSpace(
    shared_ptr<VariableSpace> parent,
    vector<int>&& domain_size,
    vector<vector<string>>&& fact_names,
    std::shared_ptr<DomainAbstraction> domain_abstraction)
    : parent(std::move(parent))
    , domain_size(std::move(domain_size))
    , fact_names(std::move(fact_names))
    , domain_abstraction(std::move(domain_abstraction))
{
}

int DomainAbstractedVariableSpace::get_num_variables() const
{
    return parent->get_num_variables();
}

int DomainAbstractedVariableSpace::get_variable_domain_size(int var) const
{
    return domain_size[var];
}

string DomainAbstractedVariableSpace::get_variable_name(int var) const
{
    return parent->get_variable_name(var);
}

string DomainAbstractedVariableSpace::get_fact_name(const FactPair& fact) const
{
    return fact_names[fact.var][fact.value];
}

DomainAbstractedOperatorSpace::DomainAbstractedOperatorSpace(
    const shared_ptr<ClassicalOperatorSpace>& parent,
    std::shared_ptr<DomainAbstraction> domain_abstraction)
    : parent(std::move(parent))
    , domain_abstraction(std::move(domain_abstraction))
{
    task_properties::verify_no_conditional_effects(*parent);
}

std::string DomainAbstractedOperatorSpace::get_operator_name(int index) const
{
    return parent->get_operator_name(index);
}

int DomainAbstractedOperatorSpace::get_num_operators() const
{
    return parent->get_num_operators();
}

int DomainAbstractedOperatorSpace::get_num_operator_preconditions(
    int index) const
{
    return parent->get_num_operator_preconditions(index);
}

int DomainAbstractedOperatorSpace::get_num_operator_effects(int op_index) const
{
    return parent->get_num_operator_effects(op_index);
}

int DomainAbstractedOperatorSpace::get_num_operator_effect_conditions(
    int op_index,
    int eff_index) const
{
    return parent->get_num_operator_effect_conditions(op_index, eff_index);
}

FactPair DomainAbstractedOperatorSpace::get_operator_precondition(
    int op_index,
    int fact_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_precondition(op_index, fact_index));
}

FactPair
DomainAbstractedOperatorSpace::get_operator_effect(int op_index, int eff_index)
    const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_effect(op_index, eff_index));
}

FactPair DomainAbstractedOperatorSpace::get_operator_effect_condition(
    int op_index,
    int eff_index,
    int cond_index) const
{
    return domain_abstraction->get_abstract_fact(
        parent->get_operator_effect_condition(op_index, eff_index, cond_index));
}

DomainAbstractedGoal::DomainAbstractedGoal(std::vector<FactPair> goals)
    : goals(std::move(goals))
{
}

int DomainAbstractedGoal::get_num_goals() const
{
    return goals.size();
}

FactPair DomainAbstractedGoal::get_goal_fact(int index) const
{
    return goals[index];
}

DomainAbstractedInitialStateValues::DomainAbstractedInitialStateValues(
    std::vector<int> initial_state_values)
    : initial_state_values(std::move(initial_state_values))
{
}

vector<int> DomainAbstractedInitialStateValues::get_initial_state_values() const
{
    return initial_state_values;
}

} // namespace downward::extra_tasks
