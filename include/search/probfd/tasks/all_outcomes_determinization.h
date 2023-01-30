#include "abstract_task.h"

#include "probfd/probabilistic_task.h"

#include "probfd/task_proxy.h"

namespace probfd {

class AODDeterminizationTask : public AbstractTask {
    const ProbabilisticTask* parent_task;

    std::vector<std::pair<int, int>> det_to_prob_index;

public:
    AODDeterminizationTask(const ProbabilisticTask* parent_task)
        : parent_task(parent_task)
    {
        ProbabilisticTaskProxy proxy(*parent_task);

        for (ProbabilisticOperatorProxy op_proxy : proxy.get_operators()) {
            const int num_outcomes = op_proxy.get_outcomes().size();
            for (int j = 0; j != num_outcomes; ++j) {
                det_to_prob_index.emplace_back(op_proxy.get_id(), j);
            }
        }
    }

    ~AODDeterminizationTask() override = default;

    int get_num_variables() const override
    {
        return parent_task->get_num_variables();
    }

    std::string get_variable_name(int var) const override
    {
        return parent_task->get_variable_name(var);
    }

    int get_variable_domain_size(int var) const override
    {
        return parent_task->get_variable_domain_size(var);
    }

    int get_variable_axiom_layer(int var) const override
    {
        return parent_task->get_variable_axiom_layer(var);
    }

    int get_variable_default_axiom_value(int var) const override
    {
        return parent_task->get_variable_default_axiom_value(var);
    }

    std::string get_fact_name(const FactPair& fact) const override
    {
        return parent_task->get_fact_name(fact);
    }

    bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const override
    {
        return parent_task->are_facts_mutex(fact1, fact2);
    }

    int get_num_axioms() const override
    {
        return parent_task->get_num_axioms();
    }

    std::string get_axiom_name(int index) const override
    {
        return parent_task->get_axiom_name(index);
    }

    int get_num_axiom_preconditions(int index) const override
    {
        return parent_task->get_num_axiom_preconditions(index);
    }

    FactPair get_axiom_precondition(int op_index, int fact_index) const override
    {
        return parent_task->get_axiom_precondition(op_index, fact_index);
    }

    int get_num_axiom_effects(int op_index) const override
    {
        return parent_task->get_num_axiom_effects(op_index);
    }

    int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const override
    {
        return parent_task->get_num_axiom_effect_conditions(
            op_index,
            eff_index);
    }

    FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const override
    {
        return parent_task->get_axiom_effect_condition(
            op_index,
            eff_index,
            cond_index);
    }

    FactPair get_axiom_effect(int op_index, int eff_index) const override
    {
        return parent_task->get_axiom_effect(op_index, eff_index);
    }

    int get_operator_cost(int index) const override
    {
        return parent_task->get_operator_cost(get_parent_indices(index).first);
    }

    std::string get_operator_name(int index) const override
    {
        return parent_task->get_operator_name(get_parent_indices(index).first);
    }

    int get_num_operators() const override { return det_to_prob_index.size(); }

    int get_num_operator_preconditions(int index) const override
    {
        return parent_task->get_num_operator_preconditions(
            get_parent_indices(index).first);
    }

    FactPair
    get_operator_precondition(int op_index, int fact_index) const override
    {
        return parent_task->get_operator_precondition(
            get_parent_indices(op_index).first,
            fact_index);
    }

    int get_num_operator_effects(int op_index) const override
    {
        const auto& [pr_op_index, pr_outcome_index] =
            get_parent_indices(op_index);
        return parent_task->get_num_operator_outcome_effects(
            pr_op_index,
            pr_outcome_index);
    }

    int get_num_operator_effect_conditions(int op_index, int eff_index)
        const override
    {
        const auto& [pr_op_index, pr_outcome_index] =
            get_parent_indices(op_index);
        return parent_task->get_num_operator_outcome_effect_conditions(
            pr_op_index,
            pr_outcome_index,
            eff_index);
    }

    FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const override
    {
        const auto& [pr_op_index, pr_outcome_index] =
            get_parent_indices(op_index);
        return parent_task->get_operator_outcome_effect_condition(
            pr_op_index,
            pr_outcome_index,
            eff_index,
            cond_index);
    }

    FactPair get_operator_effect(int op_index, int eff_index) const override
    {
        const auto& [pr_op_index, pr_outcome_index] =
            get_parent_indices(op_index);
        return parent_task->get_operator_outcome_effect(
            pr_op_index,
            pr_outcome_index,
            eff_index);
    }

    int get_num_goals() const override { return parent_task->get_num_goals(); }

    FactPair get_goal_fact(int index) const override
    {
        return parent_task->get_goal_fact(index);
    }

    std::vector<int> get_initial_state_values() const override
    {
        return parent_task->get_initial_state_values();
    }

    /*
     * Convert state values from an ancestor task A (ancestor) into
     * state values from this task, C (child). Task A has to be an
     * ancestor of C in the sense that C is the result of a sequence of
     * task transformations on A.
     * The values are converted in-place to avoid unnecessary copies. If a
     * subclass needs to create a new vector, e.g., because the size changes,
     * it should create the new vector in a local variable and then swap it with
     * the parameter.
     */
    void convert_ancestor_state_values(
        std::vector<int>&,
        const AbstractTaskBase* ancestor_task) const override
    {
        (void)ancestor_task;
        assert(ancestor_task == this || ancestor_task == parent_task);
    }
    /*
     * Convert an operator index from this task, C (child), into an operator
     * index from an ancestor task A (ancestor). Task A has to be an ancestor of
     * C in the sense that C is the result of a sequence of task transformations
     * on A.
     */
    int convert_operator_index(int index, const AbstractTaskBase* ancestor_task)
        const override
    {
        (void)ancestor_task;
        assert(ancestor_task == this);
        return index;
    }

private:
    std::pair<int, int>
    get_parent_indices(int deterministic_operator_index) const
    {
        return det_to_prob_index[deterministic_operator_index];
    }
};

} // namespace probfd