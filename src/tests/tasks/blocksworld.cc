#include "tests/tasks/blocksworld.h"

#include "probfd/tasks/explicit_termination_costs.h"

#include "downward/tasks/empty_axiom_space.h"
#include "downward/tasks/explicit_initial_state_values.h"
#include "downward/tasks/modified_goals_task.h"

#include "downward/state.h"

#include <algorithm>
#include <format>
#include <set>

using namespace probfd;
using namespace downward;

using std::ranges::sort;

namespace tests {

BlocksWorldFactIndex::BlocksWorldFactIndex(int num_blocks)
    : num_blocks(num_blocks)
{
}

int BlocksWorldFactIndex::get_num_blocks() const
{
    return num_blocks;
}

int BlocksWorldFactIndex::get_num_variables() const
{
    return 2 * num_blocks + 1;
}

int BlocksWorldFactIndex::get_clear_var(int block) const
{
    return block;
}

int BlocksWorldFactIndex::get_location_var(int block) const
{
    return num_blocks + block;
}

int BlocksWorldFactIndex::get_hand_var() const
{
    return 2 * num_blocks;
}

FactPair
BlocksWorldFactIndex::get_fact_is_block_clear(int block, bool is_clear) const
{
    return FactPair{get_clear_var(block), is_clear ? 1 : 0};
}

FactPair
BlocksWorldFactIndex::get_fact_block_on_block(int block1, int block2) const
{
    return FactPair{get_location_var(block1), block2};
}

FactPair BlocksWorldFactIndex::get_fact_block_on_table(int block) const
{
    return FactPair{get_location_var(block), num_blocks};
}

FactPair BlocksWorldFactIndex::get_fact_block_in_hand(int block) const
{
    return FactPair{get_location_var(block), num_blocks + 1};
}

FactPair BlocksWorldFactIndex::get_fact_is_hand_empty(bool is_empty) const
{
    return FactPair{get_hand_var(), is_empty ? 1 : 0};
}

State BlocksWorldFactIndex::get_state(const std::vector<FactPair>& facts) const
{
    std::vector<int> values(2 * num_blocks + 1, -1);

    for (const auto& [var, val] : facts) {
        if (values[var] != -1) abort();
        values[var] = val;
    }

    for (const int val : values) {
        if (val == -1) abort();
    }

    return State(std::move(values));
}

BlocksWorldOperatorIndex::BlocksWorldOperatorIndex(
    const BlocksWorldFactIndex& index)
    : num_blocks(index.get_num_blocks())
    , pick_up_begin(num_blocks * (num_blocks - 1) * (num_blocks - 2))
    , pick_table_begin(pick_up_begin + num_blocks * (num_blocks - 1))
    , put_tower_block_begin(pick_table_begin + num_blocks)
    , put_tower_down_begin(
          put_tower_block_begin +
          num_blocks * (num_blocks - 1) * (num_blocks - 2))
    , put_on_block_begin(put_tower_down_begin + num_blocks * (num_blocks - 1))
    , put_table_begin(put_on_block_begin + num_blocks * (num_blocks - 1))
{
}

int BlocksWorldOperatorIndex::get_num_blocks() const
{
    return num_blocks;
}

int BlocksWorldOperatorIndex::get_num_operators() const
{
    return put_table_begin + num_blocks;
}

int BlocksWorldOperatorIndex::get_operator_pick_up_tower_index(
    int b1,
    int b2,
    int b3) const
{
    assert(b1 != b2 && b1 != b3 && b2 != b3);
    if (b3 >= b1) --b3;
    if (b3 >= b2) --b3;
    if (b2 >= b1) --b2;
    return b1 * (num_blocks - 1) * (num_blocks - 2) + b2 * (num_blocks - 2) +
           b3;
}

int BlocksWorldOperatorIndex::get_operator_pick_up_block_on_block_index(
    int b1,
    int b2) const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return pick_up_begin + b1 * (num_blocks - 1) + b2;
}

int BlocksWorldOperatorIndex::get_operator_pick_up_block_from_table_index(
    int b) const
{
    return pick_table_begin + b;
}

int BlocksWorldOperatorIndex::get_operator_put_tower_on_block_index(
    int b1,
    int b2,
    int b3) const
{
    assert(b1 != b2 && b1 != b3 && b2 != b3);
    if (b3 >= b1) --b3;
    if (b3 >= b2) --b3;
    if (b2 >= b1) --b2;
    return put_tower_block_begin + b1 * (num_blocks - 1) * (num_blocks - 2) +
           b2 * (num_blocks - 2) + b3;
}

int BlocksWorldOperatorIndex::get_operator_put_tower_on_table_index(
    int b1,
    int b2) const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return put_tower_down_begin + b1 * (num_blocks - 1) + b2;
}

int BlocksWorldOperatorIndex::get_operator_put_block_on_block_index(
    int b1,
    int b2) const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return put_on_block_begin + b1 * (num_blocks - 1) + b2;
}

int BlocksWorldOperatorIndex::get_operator_put_block_on_table_index(int b) const
{
    return put_table_begin + b;
}

int BlocksWorldOperatorIndex::get_operator_outcome_id(
    int index,
    int outcome_index) const
{
    if (index < pick_up_begin) {
        return 2 * index + outcome_index;
    } else if (index < pick_table_begin) {
        return 2 * index + outcome_index;
    } else if (index < put_tower_block_begin) {
        return 2 * index + outcome_index;
    } else if (index < put_tower_down_begin) {
        return 2 * index + outcome_index;
    } else if (index < put_on_block_begin) {
        return 2 * put_tower_down_begin + (index - put_tower_down_begin);
    } else if (index < put_table_begin) {
        return 2 * put_tower_down_begin +
               (put_on_block_begin - put_tower_down_begin) +
               2 * (index - put_on_block_begin) + outcome_index;
    } else {
        return 2 * put_tower_down_begin +
               (put_on_block_begin - put_tower_down_begin) +
               2 * (put_table_begin - put_on_block_begin) +
               (index - put_table_begin);
    }
}

BlocksWorldVariableSpace::BlocksWorldVariableSpace(
    const BlocksWorldFactIndex& index)
    : variables(index.get_num_variables())
{
    for (int i = 0; i != index.get_num_blocks(); ++i) {
        auto& info = variables[index.get_clear_var(i)];
        info.name = std::format("Clear({})", i);
        info.fact_names.emplace_back(std::format("Clear({}) = false", i));
        info.fact_names.emplace_back(std::format("Clear({}) = true", i));
    }

    for (int i = 0; i != index.get_num_blocks(); ++i) {
        auto& info = variables[index.get_location_var(i)];
        info.name = std::format("Location({})", i);

        for (int j = 0; j != index.get_num_blocks(); ++j) {
            info.fact_names.emplace_back(
                std::format("Location({}) = {}", i, j));
        }

        info.fact_names.emplace_back(std::format("Location({}) = table", i));
        info.fact_names.emplace_back(std::format("Location({}) = hand", i));
    }

    {
        auto& info = variables[index.get_hand_var()];
        info.name = "HandEmpty";
        info.fact_names.emplace_back("HandEmpty = false");
        info.fact_names.emplace_back("HandEmpty = true");
    }
}

BlocksWorldOperatorSpace::BlocksWorldOperatorSpace(
    const BlocksWorldFactIndex& fact_index,
    const BlocksWorldOperatorIndex& operator_index)
    : operator_index(operator_index)
    , operators(operator_index.get_num_operators())
{
    const int blocks = fact_index.get_num_blocks();

    for (int b1 = 0; b1 != blocks; ++b1) {
        for (int b2 = 0; b2 != blocks; ++b2) {
            if (b1 == b2) continue;
            for (int b3 = 0; b3 != blocks; ++b3) {
                if (b1 == b3 || b2 == b3) continue;
                {
                    auto& op_info = operators
                        [operator_index
                             .get_operator_pick_up_tower_index(b1, b2, b3)];

                    op_info.name =
                        std::format("pick-tower {} {} {}", b1, b2, b3);
                    op_info.preconditions = {
                        fact_index.get_fact_is_hand_empty(true),
                        fact_index.get_fact_is_block_clear(b1, true),
                        fact_index.get_fact_block_on_block(b1, b2),
                        fact_index.get_fact_block_on_block(b2, b3)};
                    op_info.outcomes = {
                        {0.1,
                         {fact_index.get_fact_is_hand_empty(false),
                          fact_index.get_fact_block_in_hand(b2),
                          fact_index.get_fact_is_block_clear(b3, true)}},
                        {0.9, {}}};
                }
                {
                    auto& op_info =
                        operators[operator_index
                                      .get_operator_put_tower_on_block_index(
                                          b1,
                                          b2,
                                          b3)];

                    op_info.name =
                        std::format("put-tower {} {} {}", b1, b2, b3);
                    op_info.preconditions = {
                        fact_index.get_fact_block_in_hand(b2),
                        fact_index.get_fact_block_on_block(b1, b2),
                        fact_index.get_fact_is_block_clear(b3, true)};
                    op_info.outcomes = {
                        {0.1,
                         {fact_index.get_fact_is_hand_empty(true),
                          fact_index.get_fact_block_on_block(b2, b3),
                          fact_index.get_fact_is_block_clear(b3, false)}},
                        {0.9,
                         {fact_index.get_fact_is_hand_empty(true),
                          fact_index.get_fact_block_on_table(b2)}}};
                }
            }
            {
                auto& op_info = operators
                    [operator_index
                         .get_operator_pick_up_block_on_block_index(b1, b2)];

                op_info.name = std::format("pick-block {} {}", b1, b2);
                op_info.preconditions = {
                    fact_index.get_fact_is_hand_empty(true),
                    fact_index.get_fact_is_block_clear(b1, true),
                    fact_index.get_fact_block_on_block(b1, b2)};
                op_info.outcomes = {
                    {0.75,
                     {fact_index.get_fact_is_block_clear(b2, true),
                      fact_index.get_fact_block_in_hand(b1),
                      fact_index.get_fact_is_hand_empty(false)}},
                    {0.25,
                     {fact_index.get_fact_is_block_clear(b2, true),
                      fact_index.get_fact_block_on_table(b1)}}};
            }

            {
                auto& op_info = operators
                    [operator_index
                         .get_operator_put_block_on_block_index(b1, b2)];

                op_info.name = std::format("put-block {} {}", b1, b2);
                op_info.preconditions = {
                    fact_index.get_fact_block_in_hand(b1),
                    fact_index.get_fact_is_block_clear(b1, true),
                    fact_index.get_fact_is_block_clear(b2, true)};
                op_info.outcomes = {
                    {0.75,
                     {fact_index.get_fact_is_hand_empty(true),
                      fact_index.get_fact_block_on_block(b1, b2),
                      fact_index.get_fact_is_block_clear(b2, false)}},
                    {0.25,
                     {fact_index.get_fact_is_hand_empty(true),
                      fact_index.get_fact_block_on_table(b1)}}};
            }

            {
                auto& op_info = operators
                    [operator_index
                         .get_operator_put_tower_on_table_index(b1, b2)];

                op_info.name = std::format("put-tower-on-table {} {}", b1, b2);
                op_info.preconditions = {
                    fact_index.get_fact_block_in_hand(b2),
                    fact_index.get_fact_block_on_block(b1, b2)};
                op_info.outcomes = {
                    {1,
                     {fact_index.get_fact_is_hand_empty(true),
                      fact_index.get_fact_block_on_table(b2)}}};
            }
        }

        {
            auto& op_info =
                operators[operator_index
                              .get_operator_pick_up_block_from_table_index(b1)];

            op_info.name = std::format("pick-block-from-table {}", b1);
            op_info.preconditions = {
                fact_index.get_fact_is_hand_empty(true),
                fact_index.get_fact_is_block_clear(b1, true),
                fact_index.get_fact_block_on_table(b1)};
            op_info.outcomes = {
                {0.75,
                 {fact_index.get_fact_is_hand_empty(false),
                  fact_index.get_fact_block_in_hand(b1)}},
                {0.25, {}}};
        }

        {
            auto& op_info =
                operators[operator_index.get_operator_put_block_on_table_index(
                    b1)];

            op_info.name = std::format("put-block-on-table {}", b1);
            op_info.preconditions = {
                fact_index.get_fact_block_in_hand(b1),
                fact_index.get_fact_is_block_clear(b1, true)};
            op_info.outcomes = {
                {1,
                 {fact_index.get_fact_is_hand_empty(true),
                  fact_index.get_fact_block_on_table(b1)}}};
        }
    }

    // Sort fact ranges in increasing order of variable ids.
    for (OperatorInfo& op_info : operators) {
        sort(op_info.preconditions, {}, &FactPair::var);
        for (EffectInfo& effect_info : op_info.outcomes) {
            sort(effect_info.effects, {}, &FactPair::var);
        }
    }
}

int BlocksWorldVariableSpace::get_num_variables() const
{
    return static_cast<int>(variables.size());
}

std::string BlocksWorldVariableSpace::get_variable_name(int var) const
{
    return variables[var].name;
}

int BlocksWorldVariableSpace::get_variable_domain_size(int var) const
{
    return static_cast<int>(variables[var].fact_names.size());
}

std::string BlocksWorldVariableSpace::get_fact_name(const FactPair& fact) const
{
    return variables[fact.var].fact_names[fact.value];
}

std::string BlocksWorldOperatorSpace::get_operator_name(int index) const
{
    return operators[index].name;
}

int BlocksWorldOperatorSpace::get_num_operators() const
{
    return static_cast<int>(operators.size());
}

int BlocksWorldOperatorSpace::get_num_operator_preconditions(int index) const
{
    return static_cast<int>(operators[index].preconditions.size());
}

FactPair
BlocksWorldOperatorSpace::get_operator_precondition(int index, int fact_index)
    const
{
    return operators[index].preconditions[fact_index];
}

int BlocksWorldOperatorSpace::get_num_operator_outcomes(int index) const
{
    return static_cast<int>(operators[index].outcomes.size());
}

value_t BlocksWorldOperatorSpace::get_operator_outcome_probability(
    int index,
    int outcome_index) const
{
    return operators[index].outcomes[outcome_index].probability;
}

int BlocksWorldOperatorSpace::get_operator_outcome_id(
    int index,
    int outcome_index) const
{
    return operator_index.get_operator_outcome_id(index, outcome_index);
}

int BlocksWorldOperatorSpace::get_num_operator_outcome_effects(
    int index,
    int outcome_index) const
{
    return static_cast<int>(
        operators[index].outcomes[outcome_index].effects.size());
}

FactPair BlocksWorldOperatorSpace::get_operator_outcome_effect(
    int index,
    int outcome_index,
    int eff_index) const
{
    return operators[index].outcomes[outcome_index].effects[eff_index];
}

int BlocksWorldOperatorSpace::get_num_operator_outcome_effect_conditions(
    int,
    int,
    int) const
{
    return 0;
}

FactPair BlocksWorldOperatorSpace::get_operator_outcome_effect_condition(
    int,
    int,
    int,
    int) const
{
    abort();
}

BlocksWorldCostFunction::BlocksWorldCostFunction(
    const BlocksWorldFactIndex& fact_index,
    const BlocksWorldOperatorIndex& operator_index,
    value_t pick_block_cost,
    value_t pick_tower_cost,
    value_t put_block_cost,
    value_t put_tower_cost)
    : operator_costs(operator_index.get_num_operators())
{
    const int blocks = fact_index.get_num_blocks();

    for (int b1 = 0; b1 != blocks; ++b1) {
        for (int b2 = 0; b2 != blocks; ++b2) {
            if (b1 == b2) continue;
            for (int b3 = 0; b3 != blocks; ++b3) {
                if (b1 == b3 || b2 == b3) continue;

                operator_costs[operator_index.get_operator_pick_up_tower_index(
                    b1,
                    b2,
                    b3)] = pick_tower_cost;
                operator_costs
                    [operator_index
                         .get_operator_put_tower_on_block_index(b1, b2, b3)] =
                        put_tower_cost;
            }

            operator_costs[operator_index
                               .get_operator_pick_up_block_on_block_index(
                                   b1,
                                   b2)] = pick_block_cost;

            operator_costs[operator_index
                               .get_operator_put_block_on_block_index(b1, b2)] =
                put_block_cost;

            operator_costs[operator_index
                               .get_operator_put_tower_on_table_index(b1, b2)] =
                put_tower_cost;
        }

        operator_costs[operator_index
                           .get_operator_pick_up_block_from_table_index(b1)] =
            pick_block_cost;

        operator_costs[operator_index.get_operator_put_block_on_table_index(
            b1)] = put_block_cost;
    }
}

value_t BlocksWorldCostFunction::get_operator_cost(int index) const
{
    return operator_costs[index];
}

SharedProbabilisticTask create_probabilistic_blocksworld_task(
    const BlocksWorldFactIndex& fact_index,
    const BlocksWorldOperatorIndex& operator_index,
    const std::vector<std::vector<int>>& initial,
    const std::vector<std::vector<int>>& goal,
    value_t pick_block_cost,
    value_t pick_tower_cost,
    value_t put_block_cost,
    value_t put_tower_cost)
{
    /*
     * Read the initial state
     */

    std::vector<int> initial_state(fact_index.get_num_variables(), -1);

    const int blocks = fact_index.get_num_blocks();

    for (int i = 0; i != blocks; ++i) {
        // Assume all blocks are clear and in the hand in the beginning
        initial_state[fact_index.get_clear_var(i)] = 1;
        initial_state[fact_index.get_location_var(i)] = blocks + 1;
    }

    std::set<int> seen;

    for (const std::vector<int>& stack : initial) {
        if (stack.empty()) abort();

        for (size_t i = 0; i != stack.size() - 1; ++i) {
            // Blocks may not be encountered twice
            if (!seen.insert(stack[i]).second) abort();

            // This block is on the succeeding block
            initial_state[fact_index.get_location_var(stack[i])] = stack[i + 1];

            // The top of the succeeding block is not clear
            initial_state[fact_index.get_clear_var(stack[i + 1])] = 0;
        }

        // Blocks may not be encountered twice
        if (!seen.insert(stack.back()).second) abort();

        // The last block is on the table
        initial_state[fact_index.get_location_var(stack.back())] = blocks;
    }

    if (static_cast<int>(seen.size()) == blocks) {
        // All blocks placed, hand is empty
        initial_state[fact_index.get_hand_var()] = 1;
    } else {
        initial_state[fact_index.get_hand_var()] = 0;
        if (static_cast<int>(seen.size()) != blocks - 1) {
            // Only one block may be held
            abort();
        }
    }

    /*
     * Read the goal state
     */

    seen.clear();

    std::vector<FactPair> goal_facts;

    for (const std::vector<int>& stack : goal) {
        if (stack.empty()) abort();

        for (size_t i = 0; i != stack.size() - 1; ++i) {
            // Blocks may not be encountered twice
            if (!seen.insert(stack[i]).second) abort();

            // This block must be on the succeeding block
            goal_facts.emplace_back(
                fact_index.get_location_var(stack[i]),
                stack[i + 1]);
        }

        // Blocks may not be encountered twice
        if (!seen.insert(stack.back()).second) abort();

        // The last block is on the table
        goal_facts.emplace_back(
            fact_index.get_location_var(stack.back()),
            blocks);
    }

    return {
        std::make_shared<BlocksWorldVariableSpace>(fact_index),
        std::make_shared<downward::tasks::EmptyAxiomSpace>(),
        std::make_shared<BlocksWorldOperatorSpace>(fact_index, operator_index),
        std::make_shared<extra_tasks::ModifiedGoalFacts>(std::move(goal_facts)),
        std::make_shared<extra_tasks::ExplicitInitialStateValues>(
            initial_state),
        std::make_shared<BlocksWorldCostFunction>(
            fact_index,
            operator_index,
            pick_block_cost,
            pick_tower_cost,
            put_block_cost,
            put_tower_cost),
        std::make_shared<probfd::tasks::ExplicitTermininationCosts>(
            0_vt,
            INFINITE_VALUE)};
};

} // namespace tests
