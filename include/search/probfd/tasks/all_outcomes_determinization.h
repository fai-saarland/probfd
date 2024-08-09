#ifndef PROBFD_TASKS_ALL_OUTCOMES_DETERMINIZATION_H
#define PROBFD_TASKS_ALL_OUTCOMES_DETERMINIZATION_H

#include "downward/abstract_task.h"

#include <string>
#include <utility>
#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;
}

namespace probfd::tasks {

/**
 * @brief Deterministic planning task representing the all-outcomes
 * determinization of a probabilistic planning task.
 *
 * In the all-outcomes determinization, each probabilistic operator is replaced
 * by deterministic operators, one for each probabilistic outcome, with the same
 * precondition as the original operator and the same effect as the outcome
 * that induces the deterministic operator. Essentially, every probabilistic
 * outcome can be chosen at will.
 */
class AODDeterminizationTask final : public AbstractTask {
    const ProbabilisticTask* parent_task_;

    std::vector<std::pair<int, int>> det_to_prob_index_;

public:
    /// Constructs the all-outcomes determinization of the input probabilistic
    /// planning task.
    explicit AODDeterminizationTask(const ProbabilisticTask* parent_task);

    ~AODDeterminizationTask() final = default;

    int get_num_variables() const final;

    std::string get_variable_name(int var) const final;

    int get_variable_domain_size(int var) const final;

    int get_variable_axiom_layer(int var) const final;

    int get_variable_default_axiom_value(int var) const final;

    std::string get_fact_name(const FactPair& fact) const final;

    bool
    are_facts_mutex(const FactPair& fact1, const FactPair& fact2) const final;

    int get_num_axioms() const final;

    std::string get_axiom_name(int index) const final;

    int get_num_axiom_preconditions(int index) const final;

    FactPair get_axiom_precondition(int op_index, int fact_index) const final;

    int get_num_axiom_effects(int op_index) const final;

    int
    get_num_axiom_effect_conditions(int op_index, int eff_index) const final;

    FactPair
    get_axiom_effect_condition(int op_index, int eff_index, int cond_index)
        const final;

    FactPair get_axiom_effect(int op_index, int eff_index) const final;

    int get_operator_cost(int index) const final;

    std::string get_operator_name(int index) const final;

    int get_num_operators() const final;

    int get_num_operator_preconditions(int index) const final;

    FactPair
    get_operator_precondition(int op_index, int fact_index) const final;

    int get_num_operator_effects(int op_index) const final;

    int
    get_num_operator_effect_conditions(int op_index, int eff_index) const final;

    FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const final;

    FactPair get_operator_effect(int op_index, int eff_index) const final;

    int get_num_goals() const final;

    FactPair get_goal_fact(int index) const final;

    std::vector<int> get_initial_state_values() const final;

    void convert_ancestor_state_values(
        std::vector<int>&,
        const PlanningTask* ancestor_task) const final;

    int convert_operator_index(int index, const PlanningTask* ancestor_task)
        const final;

private:
    std::pair<int, int>
    get_parent_indices(int deterministic_operator_index) const;
};

} // namespace probfd::tasks

#endif