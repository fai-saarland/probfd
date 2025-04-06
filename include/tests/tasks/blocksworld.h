#ifndef TESTS_TASKS_BLOCKSWORLD_H
#define TESTS_TASKS_BLOCKSWORLD_H

#include "probfd/probabilistic_task.h"

#include "probfd/value_type.h"

#include <vector>

namespace downward {
class State;
}

namespace tests {

class BlocksworldTask : public probfd::ProbabilisticTask {
    const int blocks;

    const int pick_up_begin;
    const int pick_table_begin;
    const int put_tower_block_begin;
    const int put_tower_down_begin;
    const int put_on_block_begin;
    const int put_table_begin;

    struct VariableInfo {
        std::string name;
        std::vector<std::string> fact_names;
    };

    struct EffectInfo {
        probfd::value_t probability;
        std::vector<downward::FactPair> effects;
    };

    struct OperatorInfo {
        std::string name;
        probfd::value_t cost;
        std::vector<downward::FactPair> preconditions;
        std::vector<EffectInfo> outcomes;
    };

    std::vector<VariableInfo> variables;
    std::vector<OperatorInfo> operators;

    std::vector<int> initial_state;
    std::vector<downward::FactPair> goal_state;

public:
    BlocksworldTask(
        int num_blocks,
        const std::vector<std::vector<int>>& initial,
        const std::vector<std::vector<int>>& goal,
        probfd::value_t pick_block_cost = 1,
        probfd::value_t pick_tower_cost = 0,
        probfd::value_t put_block_cost = 0,
        probfd::value_t put_tower_cost = 0);

    int get_num_variables() const override;

    std::string get_variable_name(int var) const override;

    int get_variable_domain_size(int var) const override;

    int get_variable_axiom_layer(int var) const override;

    int get_variable_default_axiom_value(int var) const override;

    std::string get_fact_name(const downward::FactPair& fact) const override;

    bool are_facts_mutex(const downward::FactPair&, const downward::FactPair&)
        const override;

    int get_num_axioms() const override;

    std::string get_axiom_name(int) const override;

    int get_num_axiom_preconditions(int) const override;

    downward::FactPair get_axiom_precondition(int, int) const override;

    int get_num_axiom_effects(int) const override;

    int get_num_axiom_effect_conditions(int, int) const override;

    downward::FactPair get_axiom_effect_condition(int, int, int) const override;

    downward::FactPair get_axiom_effect(int, int) const override;

    std::string get_operator_name(int index) const override;

    int get_num_operators() const override;

    int get_num_operator_preconditions(int index) const override;

    downward::FactPair
    get_operator_precondition(int index, int fact_index) const override;

    int get_num_goals() const override;

    downward::FactPair get_goal_fact(int index) const override;

    std::vector<int> get_initial_state_values() const override;

    void convert_ancestor_state_values(std::vector<int>&, const PlanningTask*)
        const override;

    int convert_operator_index(int, const PlanningTask*) const override;

    probfd::value_t get_operator_cost(int index) const override;

    int get_num_operator_outcomes(int index) const override;

    probfd::value_t
    get_operator_outcome_probability(int index, int outcome_index)
        const override;

    int get_operator_outcome_id(int index, int outcome_index) const override;

    int get_num_operator_outcome_effects(int index, int outcome_index)
        const override;

    downward::FactPair
    get_operator_outcome_effect(int index, int outcome_index, int eff_index)
        const override;

    int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const override;

    downward::FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const override;

    probfd::value_t get_goal_termination_cost() const override;
    probfd::value_t get_non_goal_termination_cost() const override;

    int get_clear_var(int block) const;
    int get_location_var(int block) const;
    int get_hand_var() const;

    downward::FactPair get_fact_is_block_clear(int block, bool is_clear) const;
    downward::FactPair get_fact_block_on_block(int block1, int block2) const;
    downward::FactPair get_fact_block_on_table(int block) const;
    downward::FactPair get_fact_block_in_hand(int block) const;
    downward::FactPair get_fact_is_hand_empty(bool is_empty) const;

    int get_operator_pick_up_tower_index(int b1, int b2, int b3) const;
    int get_operator_pick_up_block_on_block_index(int b1, int b2) const;
    int get_operator_pick_up_block_from_table_index(int b) const;

    int get_operator_put_tower_on_block_index(int b1, int b2, int b3) const;
    int get_operator_put_tower_on_table_index(int b1, int b2) const;
    int get_operator_put_block_on_block_index(int b1, int b2) const;
    int get_operator_put_block_on_table_index(int b) const;

    downward::State get_state(const std::vector<downward::FactPair>& facts);
};

} // namespace tests

#endif