#ifndef PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H
#define PROBFD_TASKS_DOMAIN_ABSTRACTED_TASK_H

#include "probfd/tasks/delegating_task.h"

#include "utils/collections.h"

#include <string>
#include <utility>
#include <vector>

namespace probfd {
namespace extra_tasks {

/*
  Task transformation for performing domain abstraction.

  We recommend using the factory function in
  domain_abstracted_task_factory.h for creating DomainAbstractedTasks.
*/
class DomainAbstractedTask : public tasks::DelegatingTask {
    const std::vector<int> domain_size;
    const std::vector<int> initial_state_values;
    const std::vector<FactPair> goals;
    const std::vector<std::vector<std::string>> fact_names;
    const std::vector<std::vector<int>> value_map;

    FactPair get_abstract_fact(const FactPair& fact) const;

public:
    DomainAbstractedTask(
        const std::shared_ptr<ProbabilisticTask>& parent,
        std::vector<int>&& domain_size,
        std::vector<int>&& initial_state_values,
        std::vector<FactPair>&& goals,
        std::vector<std::vector<std::string>>&& fact_names,
        std::vector<std::vector<int>>&& value_map);

    int get_variable_domain_size(int var) const override;
    std::string get_fact_name(const FactPair& fact) const override;
    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    FactPair get_axiom_effect(int op_index, int eff_index) const override;

    FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const override;

    FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;
    void
    convert_state_values_from_parent(std::vector<int>& values) const override;
};

} // namespace extra_tasks
} // namespace probfd

#endif
