#ifndef TASKS_DOMAIN_ABSTRACTED_TASK_H
#define TASKS_DOMAIN_ABSTRACTED_TASK_H

#include "downward/tasks/delegating_task.h"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

class DomainAbstraction;

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

    const std::shared_ptr<DomainAbstraction> domain_abstraction;

public:
    DomainAbstractedTask(
        const std::shared_ptr<AbstractTask>& parent,
        std::vector<int>&& domain_size,
        std::vector<int>&& initial_state_values,
        std::vector<FactPair>&& goals,
        std::vector<std::vector<std::string>>&& fact_names,
        std::shared_ptr<DomainAbstraction> domain_abstraction);

    virtual int get_variable_domain_size(int var) const override;
    virtual std::string get_fact_name(const FactPair& fact) const override;

    virtual FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    virtual FactPair
    get_axiom_effect(int op_index, int eff_index) const override;

    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const override;
    virtual FactPair
    get_operator_effect(int op_index, int eff_index) const override;

    virtual FactPair get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;
};
} // namespace extra_tasks

#endif
