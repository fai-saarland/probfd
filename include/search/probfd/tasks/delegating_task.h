#ifndef PROBFD_TASKS_DELEGATING_TASK_H
#define PROBFD_TASKS_DELEGATING_TASK_H

#include "probfd/probabilistic_task.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace probfd {
namespace tasks {

class DelegatingTask : public ProbabilisticTask {
protected:
    const std::shared_ptr<ProbabilisticTask> parent;

public:
    explicit DelegatingTask(const std::shared_ptr<ProbabilisticTask>& parent);
    ~DelegatingTask() override = default;

    int get_num_variables() const override;
    std::string get_variable_name(int var) const override;
    int get_variable_domain_size(int var) const override;
    int get_variable_axiom_layer(int var) const override;
    int get_variable_default_axiom_value(int var) const override;
    std::string get_fact_name(const FactPair& fact) const override;
    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;

    int get_num_axioms() const override;

    std::string get_axiom_name(int index) const override;
    int get_num_axiom_preconditions(int index) const override;
    FactPair
    get_axiom_precondition(int op_index, int fact_index) const override;
    int get_num_axiom_effects(int op_index) const override;
    int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const override;
    FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const override;
    FactPair get_axiom_effect(int op_index, int eff_index) const override;

    value_t get_operator_cost(int index) const override;
    std::string get_operator_name(int index) const override;
    int get_num_operators() const override;
    int get_num_operator_preconditions(int index) const override;
    FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    int get_num_goals() const override;
    FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;

    void convert_ancestor_state_values(
        std::vector<int>& values,
        const AbstractTaskBase* ancestor_task) const override;

    int convert_operator_index(int index, const AbstractTaskBase* ancestor_task)
        const override;

    int get_num_operator_outcomes(int op_index) const override;

    value_t get_operator_outcome_probability(int op_index, int outcome_index)
        const override;

    int get_operator_outcome_id(int op_index, int outcome_index) const override;

    int get_num_operator_outcome_effects(int op_index, int outcome_index)
        const override;

    FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const override;

    int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const override;

    FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const override;

    virtual void convert_state_values_from_parent(std::vector<int>&) const {}

    virtual int convert_operator_index_to_parent(int index) const
    {
        return index;
    }
};

} // namespace tasks
} // namespace probfd

#endif
