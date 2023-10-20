#ifndef TESTS_TASKS_BLOCKSWORLD_H
#define TESTS_TASKS_BLOCKSWORLD_H

#include "probfd/probabilistic_task.h"

#include <vector>

namespace tests {

class BlocksworldTask : public probfd::ProbabilisticTask {
    int blocks;
    std::vector<int> initial_state;
    std::vector<FactPair> goal_state;

    const int pick_tower_begin;
    const int pick_up_begin;
    const int pick_table_begin;

    const int put_tower_block_begin;
    const int put_tower_down_begin;
    const int put_on_block_begin;
    const int put_table_begin;

    const int num_operators;

    int get_clear_var(int block) const;
    int get_location_var(int block) const;
    int get_hand_var() const;

public:
    BlocksworldTask(
        int num_blocks,
        const std::vector<std::vector<int>>& initial,
        const std::vector<std::vector<int>>& goal);

    ~BlocksworldTask() override = default;

    int get_num_variables() const override;

    std::string get_variable_name(int var) const override;

    int get_variable_domain_size(int var) const override;

    int get_variable_axiom_layer(int var) const override;

    int get_variable_default_axiom_value(int var) const override;

    std::string get_fact_name(const FactPair& fact) const override;

    bool are_facts_mutex(const FactPair&, const FactPair&) const override;

    int get_num_axioms() const override;

    std::string get_axiom_name(int) const override;

    int get_num_axiom_preconditions(int) const override;

    FactPair get_axiom_precondition(int, int) const override;

    int get_num_axiom_effects(int) const override;

    int get_num_axiom_effect_conditions(int, int) const override;

    FactPair get_axiom_effect_condition(int, int, int) const override;

    FactPair get_axiom_effect(int, int) const override;

    std::string get_operator_name(int index) const override;

    int get_num_operators() const override;

    int get_num_operator_preconditions(int index) const override;

    FactPair
    get_operator_precondition(int index, int fact_index) const override;

    int get_num_goals() const override;

    FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;

    void
    convert_ancestor_state_values(std::vector<int>&, const AbstractTaskBase*)
        const override;

    int convert_operator_index(int, const AbstractTaskBase*) const override;

    probfd::value_t get_operator_cost(int index) const override;

    int get_num_operator_outcomes(int index) const override;

    probfd::value_t
    get_operator_outcome_probability(int index, int outcome_index)
        const override;

    int get_operator_outcome_id(int index, int outcome_index) const override;

    int get_num_operator_outcome_effects(int index, int outcome_index)
        const override;

    FactPair
    get_operator_outcome_effect(int index, int outcome_index, int eff_index)
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
};

} // namespace tests

#endif