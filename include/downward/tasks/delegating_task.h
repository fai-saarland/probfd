#ifndef TASKS_DELEGATING_TASK_H
#define TASKS_DELEGATING_TASK_H

#include "downward/abstract_task.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace downward::tasks {
/*
  Task transformation that delegates all calls to the corresponding methods of
  the parent task. You should inherit from this class instead of AbstractTask
  if you need specialized behavior for only some of the methods.
*/
class DelegatingTask : public AbstractTask {
protected:
    const std::shared_ptr<AbstractTask> parent;

public:
    explicit DelegatingTask(const std::shared_ptr<AbstractTask>& parent);
    virtual ~DelegatingTask() override = default;

    virtual int get_num_variables() const override;
    virtual std::string get_variable_name(int var) const override;
    virtual int get_variable_domain_size(int var) const override;
    virtual int get_variable_axiom_layer(int var) const override;
    virtual int get_variable_default_axiom_value(int var) const override;
    virtual std::string get_fact_name(const FactPair& fact) const override;

    virtual int get_num_axioms() const override;

    virtual std::string get_axiom_name(int index) const override;
    virtual int get_num_axiom_preconditions(int index) const override;
    virtual FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    virtual int get_num_axiom_effects(int op_index) const override;
    virtual int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const override;
    virtual FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
    virtual FactPair
    get_axiom_effect(int op_index, int eff_index) const override;

    virtual int get_operator_cost(int index) const override;
    virtual std::string get_operator_name(int index) const override;
    virtual int get_num_operators() const override;
    virtual int get_num_operator_preconditions(int index) const override;
    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const override;
    virtual int get_num_operator_effects(int op_index) const override;
    virtual int get_num_operator_effect_conditions(int op_index, int eff_index)
        const override;
    virtual FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
    virtual FactPair
    get_operator_effect(int op_index, int eff_index) const override;

    virtual int get_num_goals() const override;
    virtual FactPair get_goal_fact(int index) const override;

    virtual std::vector<int> get_initial_state_values() const override;
};
} // namespace tasks

#endif
