#ifndef PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H
#define PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H

#include "probfd/tasks/delegating_task.h" // IWYU pragma: export

#include <memory>
#include <string>
#include <vector>

namespace probfd::extra_tasks {

/*
  Task transformation for performing domain abstraction.

  We recommend using the factory function in
  domain_abstracted_task_factory.h for creating DomainAbstractedTasks.
*/
class DomainAbstractedTask final : public tasks::DelegatingTask {
    const std::vector<int> domain_size_;
    const std::vector<int> initial_state_values_;
    const std::vector<downward::FactPair> goals_;
    const std::vector<std::vector<std::string>> fact_names_;
    const std::vector<std::vector<int>> value_map_;

    downward::FactPair get_abstract_fact(const downward::FactPair& fact) const;

public:
    DomainAbstractedTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        std::vector<int>&& domain_size,
        std::vector<int>&& initial_state_values,
        std::vector<downward::FactPair>&& goals,
        std::vector<std::vector<std::string>>&& fact_names,
        std::vector<std::vector<int>>&& value_map);

    value_t get_goal_termination_cost() const override;
    value_t get_non_goal_termination_cost() const override;

    int get_variable_domain_size(int var) const override;
    std::string get_fact_name(const downward::FactPair& fact) const override;

    downward::FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    downward::FactPair
    get_axiom_effect(int op_index, int eff_index) const override;

    downward::FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    downward::FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const override;

    downward::FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;
    void
    convert_state_values_from_parent(std::vector<int>& values) const override;
};

} // namespace probfd::extra_tasks

#endif
