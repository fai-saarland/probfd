#ifndef TASKS_DOMAIN_ABSTRACTED_TASK_H
#define TASKS_DOMAIN_ABSTRACTED_TASK_H

#include "downward/classical_operator_space.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/variable_space.h"

#include <string>
#include <vector>

namespace downward {
class DomainAbstraction;
}

namespace downward::extra_tasks {
/*
  Task transformation for performing domain abstraction.

  We recommend using the factory function in
  domain_abstracted_task_factory.h for creating DomainAbstractedTasks.
*/
class DomainAbstractedVariableSpace : public VariableSpace {
    std::shared_ptr<VariableSpace> parent;
    const std::vector<int> domain_size;
    const std::vector<std::vector<std::string>> fact_names;

    const std::shared_ptr<DomainAbstraction> domain_abstraction;

public:
    DomainAbstractedVariableSpace(
        std::shared_ptr<VariableSpace> parent,
        std::vector<int>&& domain_size,
        std::vector<std::vector<std::string>>&& fact_names,
        std::shared_ptr<DomainAbstraction> domain_abstraction);

    int get_num_variables() const override;
    int get_variable_domain_size(int var) const override;

    std::string get_variable_name(int var) const override;
    std::string get_fact_name(const FactPair& fact) const override;
};

class DomainAbstractedOperatorSpace : public ClassicalOperatorSpace {
    const std::shared_ptr<ClassicalOperatorSpace> parent;
    const std::shared_ptr<DomainAbstraction> domain_abstraction;

public:
    DomainAbstractedOperatorSpace(
        const std::shared_ptr<ClassicalOperatorSpace>& parent,
        std::shared_ptr<DomainAbstraction> domain_abstraction);

    std::string get_operator_name(int index) const override;
    int get_num_operators() const override;
    int get_num_operator_preconditions(int index) const override;
    int get_num_operator_effects(int op_index) const override;
    int get_num_operator_effect_conditions(int op_index, int eff_index)
        const override;

    FactPair
    get_operator_precondition(int op_index, int fact_index) const override;
    FactPair get_operator_effect(int op_index, int eff_index) const override;
    FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
};

class DomainAbstractedGoal : public GoalFactList {
    const std::vector<FactPair> goals;

public:
    explicit DomainAbstractedGoal(std::vector<FactPair> goals);

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;
};

class DomainAbstractedInitialStateValues : public InitialStateValues {
    const std::vector<int> initial_state_values;

public:
    explicit DomainAbstractedInitialStateValues(
        std::vector<int> initial_state_values);

    std::vector<int> get_initial_state_values() const override;
};

} // namespace downward::extra_tasks

#endif
