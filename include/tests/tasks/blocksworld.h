#ifndef TESTS_TASKS_BLOCKSWORLD_H
#define TESTS_TASKS_BLOCKSWORLD_H

#include "downward/axiom_space.h"
#include "downward/fact_pair.h"
#include "downward/operator_cost_function.h"
#include "downward/variable_space.h"

#include "probfd/probabilistic_operator_space.h"

#include "probfd/probabilistic_task.h"
#include "probfd/termination_costs.h"

#include "probfd/value_type.h"

#include <vector>

namespace downward {
class State;
}

namespace tests {

class BlocksWorldFactIndex {
    const int num_blocks;

public:
    explicit BlocksWorldFactIndex(int num_blocks);

    int get_num_blocks() const;

    int get_num_variables() const;

    int get_clear_var(int block) const;
    int get_location_var(int block) const;
    int get_hand_var() const;

    downward::FactPair get_fact_is_block_clear(int block, bool is_clear) const;
    downward::FactPair get_fact_block_on_block(int block1, int block2) const;
    downward::FactPair get_fact_block_on_table(int block) const;
    downward::FactPair get_fact_block_in_hand(int block) const;
    downward::FactPair get_fact_is_hand_empty(bool is_empty) const;

    downward::State
    get_state(const std::vector<downward::FactPair>& facts) const;
};

class BlocksWorldOperatorIndex {
    const int num_blocks;

    const int pick_up_begin;
    const int pick_table_begin;
    const int put_tower_block_begin;
    const int put_tower_down_begin;
    const int put_on_block_begin;
    const int put_table_begin;

public:
    explicit BlocksWorldOperatorIndex(const BlocksWorldFactIndex& index);

    int get_num_blocks() const;

    int get_num_operators() const;

    int get_operator_pick_up_tower_index(int b1, int b2, int b3) const;
    int get_operator_pick_up_block_on_block_index(int b1, int b2) const;
    int get_operator_pick_up_block_from_table_index(int b) const;

    int get_operator_put_tower_on_block_index(int b1, int b2, int b3) const;
    int get_operator_put_tower_on_table_index(int b1, int b2) const;
    int get_operator_put_block_on_block_index(int b1, int b2) const;
    int get_operator_put_block_on_table_index(int b) const;

    int get_operator_outcome_id(int index, int outcome_index) const;
};

class BlocksWorldVariableSpace : public downward::VariableSpace {
    struct VariableInfo {
        std::string name;
        std::vector<std::string> fact_names;
    };

    std::vector<VariableInfo> variables;

public:
    explicit BlocksWorldVariableSpace(const BlocksWorldFactIndex& index);

    int get_num_variables() const override;

    std::string get_variable_name(int var) const override;

    int get_variable_domain_size(int var) const override;

    std::string get_fact_name(const downward::FactPair& fact) const override;
};

class BlocksWorldOperatorSpace : public probfd::ProbabilisticOperatorSpace {
    struct EffectInfo {
        probfd::value_t probability;
        std::vector<downward::FactPair> effects;
    };

    struct OperatorInfo {
        std::string name;
        std::vector<downward::FactPair> preconditions;
        std::vector<EffectInfo> outcomes;
    };

    BlocksWorldOperatorIndex operator_index;
    std::vector<OperatorInfo> operators;

public:
    BlocksWorldOperatorSpace(
        const BlocksWorldFactIndex& fact_index,
        const BlocksWorldOperatorIndex& operator_index);

    std::string get_operator_name(int index) const override;

    int get_num_operators() const override;

    int get_num_operator_preconditions(int index) const override;

    downward::FactPair
    get_operator_precondition(int index, int fact_index) const override;

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
};

class BlocksWorldCostFunction
    : public downward::OperatorCostFunction<probfd::value_t> {
    std::vector<probfd::value_t> operator_costs;

public:
    BlocksWorldCostFunction(
        const BlocksWorldFactIndex& fact_index,
        const BlocksWorldOperatorIndex& operator_index,
        probfd::value_t pick_block_cost = 1,
        probfd::value_t pick_tower_cost = 0,
        probfd::value_t put_block_cost = 0,
        probfd::value_t put_tower_cost = 0);

    probfd::value_t get_operator_cost(int index) const override;
};

extern probfd::SharedProbabilisticTask create_probabilistic_blocksworld_task(
    const BlocksWorldFactIndex& fact_index,
    const BlocksWorldOperatorIndex& operator_index,
    const std::vector<std::vector<int>>& initial,
    const std::vector<std::vector<int>>& goal,
    probfd::value_t pick_block_cost = 1,
    probfd::value_t pick_tower_cost = 0,
    probfd::value_t put_block_cost = 0,
    probfd::value_t put_tower_cost = 0);

} // namespace tests

#endif