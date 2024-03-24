#include "tests/tasks/blocksworld.h"
#include "downward/task_proxy.h"

#include <format>
#include <set>

using namespace probfd;

namespace tests {

BlocksworldTask::BlocksworldTask(
    int num_blocks,
    const std::vector<std::vector<int>>& initial,
    const std::vector<std::vector<int>>& goal)
    : blocks(num_blocks)
    , pick_up_begin(blocks * (blocks - 1) * (blocks - 2))
    , pick_table_begin(pick_up_begin + blocks * (blocks - 1))
    , put_tower_block_begin(pick_table_begin + blocks)
    , put_tower_down_begin(
          put_tower_block_begin + blocks * (blocks - 1) * (blocks - 2))
    , put_on_block_begin(put_tower_down_begin + blocks * (blocks - 1))
    , put_table_begin(put_on_block_begin + blocks * (blocks - 1))
    , variables(2 * blocks + 1)
    , operators(put_table_begin + blocks)
    , initial_state(2 * blocks + 1, 0)
{
    /*
     * Read the initial state
     */

    for (int i = 0; i != blocks; ++i) {
        // Assume all blocks are clear and in the hand in the beginning
        initial_state[get_clear_var(i)] = 1;
        initial_state[get_location_var(i)] = blocks + 1;
    }

    std::set<int> seen;

    for (const std::vector<int>& stack : initial) {
        if (stack.empty()) abort();

        for (size_t i = 0; i != stack.size() - 1; ++i) {
            // Blocks may not be encountered twice
            if (!seen.insert(stack[i]).second) abort();

            // This block is on the succeeding block
            initial_state[get_location_var(stack[i])] = stack[i + 1];

            // The top of the succeeding block is not clear
            initial_state[get_clear_var(stack[i + 1])] = 0;
        }

        // Blocks may not be encountered twice
        if (!seen.insert(stack.back()).second) abort();

        // The last block is on the table
        initial_state[get_location_var(stack.back())] = blocks;
    }

    if (static_cast<int>(seen.size()) == blocks) {
        // All blocks placed, hand is empty
        initial_state[get_hand_var()] = 1;
    } else {
        initial_state[get_hand_var()] = 0;
        if (static_cast<int>(seen.size()) != blocks - 1) {
            // Only one block may be held
            abort();
        }
    }

    /*
     * Read the goal state
     */

    seen.clear();

    for (const std::vector<int>& stack : goal) {
        if (stack.empty()) abort();

        // This block must be clear
        goal_state.emplace_back(get_clear_var(stack.front()), 1);

        for (size_t i = 0; i != stack.size() - 1; ++i) {
            // Blocks may not be encountered twice
            if (!seen.insert(stack[i]).second) abort();

            // This block must be on the succeeding block
            goal_state.emplace_back(get_location_var(stack[i]), stack[i + 1]);
        }

        // Blocks may not be encountered twice
        if (!seen.insert(stack.back()).second) abort();

        // The last block is on the table
        goal_state.emplace_back(get_location_var(stack.back()), blocks);
    }

    // Hand must be empty
    goal_state.emplace_back(get_hand_var(), 1);

    for (int i = 0; i != blocks; ++i) {
        auto& info = variables[get_clear_var(i)];
        info.name = std::format("Clear({})", i);
        info.fact_names.emplace_back(std::format("Clear({}) = false", i));
        info.fact_names.emplace_back(std::format("Clear({}) = true", i));
    }

    for (int i = 0; i != blocks; ++i) {
        auto& info = variables[get_location_var(i)];
        info.name = std::format("Location({})", i);

        for (int j = 0; j != blocks; ++j) {
            info.fact_names.emplace_back(
                std::format("Location({}) = {}", i, j));
        }

        info.fact_names.emplace_back(std::format("Location({}) = table", i));
        info.fact_names.emplace_back(std::format("Location({}) = hand", i));
    }

    {
        auto& info = variables[get_hand_var()];
        info.name = "HandEmpty";
        info.fact_names.emplace_back("HandEmpty = false");
        info.fact_names.emplace_back("HandEmpty = true");
    }

    for (int b1 = 0; b1 != blocks; ++b1) {
        for (int b2 = 0; b2 != blocks; ++b2) {
            if (b1 == b2) continue;
            for (int b3 = 0; b3 != blocks; ++b3) {
                if (b1 == b3 || b2 == b3) continue;
                {
                    auto& op_info =
                        operators[get_operator_pick_up_tower_index(b1, b2, b3)];

                    op_info.name =
                        std::format("pick-tower {} {} {}", b1, b2, b3);
                    op_info.cost = 0;
                    op_info.preconditions = {
                        get_fact_is_hand_empty(true),
                        get_fact_is_block_clear(b1, true),
                        get_fact_block_on_block(b1, b2),
                        get_fact_block_on_block(b2, b3)};
                    op_info.outcomes = {
                        {0.1,
                         {get_fact_is_hand_empty(false),
                          get_fact_block_in_hand(b2),
                          get_fact_is_block_clear(b3, true)}},
                        {0.9, {}}};
                }
                {
                    auto& op_info = operators
                        [get_operator_put_tower_on_block_index(b1, b2, b3)];

                    op_info.name =
                        std::format("put-tower {} {} {}", b1, b2, b3);
                    op_info.cost = 0;
                    op_info.preconditions = {
                        get_fact_block_in_hand(b2),
                        get_fact_block_on_block(b1, b2),
                        get_fact_is_block_clear(b3, true)};
                    op_info.outcomes = {
                        {0.1,
                         {get_fact_is_hand_empty(true),
                          get_fact_block_on_block(b2, b3),
                          get_fact_is_block_clear(b3, false)}},
                        {0.9,
                         {get_fact_is_hand_empty(true),
                          get_fact_block_on_table(b2)}}};
                }
            }
            {
                auto& op_info = operators
                    [get_operator_pick_up_block_on_block_index(b1, b2)];

                op_info.name = std::format("pick-block {} {}", b1, b2);
                op_info.cost = 1;
                op_info.preconditions = {
                    get_fact_is_hand_empty(true),
                    get_fact_is_block_clear(b1, true),
                    get_fact_block_on_block(b1, b2)};
                op_info.outcomes = {
                    {0.75,
                     {get_fact_is_block_clear(b2, true),
                      get_fact_block_in_hand(b1),
                      get_fact_is_hand_empty(false)}},
                    {0.25,
                     {get_fact_is_block_clear(b2, true),
                      get_fact_block_on_table(b1)}}};
            }

            {
                auto& op_info =
                    operators[get_operator_put_block_on_block_index(b1, b2)];

                op_info.name = std::format("put-block {} {}", b1, b2);
                op_info.cost = 0;
                op_info.preconditions = {
                    get_fact_block_in_hand(b1),
                    get_fact_is_block_clear(b1, true),
                    get_fact_is_block_clear(b2, true)};
                op_info.outcomes = {
                    {0.75,
                     {get_fact_is_hand_empty(true),
                      get_fact_block_on_block(b1, b2),
                      get_fact_is_block_clear(b2, false)}},
                    {0.25,
                     {get_fact_is_hand_empty(true),
                      get_fact_block_on_table(b1)}}};
            }

            {
                auto& op_info =
                    operators[get_operator_put_tower_on_table_index(b1, b2)];

                op_info.name = std::format("put-tower-on-table {} {}", b1, b2);
                op_info.cost = 0;
                op_info.preconditions = {
                    get_fact_block_in_hand(b2),
                    get_fact_block_on_block(b1, b2)};
                op_info.outcomes = {
                    {1,
                     {get_fact_is_hand_empty(false),
                      get_fact_block_on_table(b2)}}};
            }
        }

        {
            auto& op_info =
                operators[get_operator_pick_up_block_from_table_index(b1)];

            op_info.name = std::format("pick-block-from-table {}", b1);
            op_info.cost = 1;
            op_info.preconditions = {
                get_fact_is_hand_empty(true),
                get_fact_is_block_clear(b1, true),
                get_fact_block_on_table(b1)};
            op_info.outcomes = {
                {0.75,
                 {get_fact_is_hand_empty(false), get_fact_block_in_hand(b1)}},
                {0.25, {}}};
        }

        {
            auto& op_info =
                operators[get_operator_put_block_on_table_index(b1)];

            op_info.name = std::format("put-block-on-table {}", b1);
            op_info.cost = 0;
            op_info.preconditions = {
                get_fact_block_in_hand(b1),
                get_fact_is_block_clear(b1, true)};
            op_info.outcomes = {
                {1,
                 {get_fact_is_hand_empty(false), get_fact_block_on_table(b1)}}};
        }
    }
}

int BlocksworldTask::get_num_variables() const
{
    return static_cast<int>(variables.size());
}

std::string BlocksworldTask::get_variable_name(int var) const
{
    return variables[var].name;
}

int BlocksworldTask::get_variable_domain_size(int var) const
{
    return static_cast<int>(variables[var].fact_names.size());
}

int BlocksworldTask::get_variable_axiom_layer(int) const
{
    return -1;
}

int BlocksworldTask::get_variable_default_axiom_value(int) const
{
    return 0;
}

std::string BlocksworldTask::get_fact_name(const FactPair& fact) const
{
    return variables[fact.var].fact_names[fact.value];
}

bool BlocksworldTask::are_facts_mutex(const FactPair&, const FactPair&) const
{
    return false;
}

int BlocksworldTask::get_num_axioms() const
{
    return 0;
}

std::string BlocksworldTask::get_axiom_name(int) const
{
    abort();
}

int BlocksworldTask::get_num_axiom_preconditions(int) const
{
    abort();
}

FactPair BlocksworldTask::get_axiom_precondition(int, int) const
{
    abort();
}

int BlocksworldTask::get_num_axiom_effects(int) const
{
    abort();
}

int BlocksworldTask::get_num_axiom_effect_conditions(int, int) const
{
    abort();
}

FactPair BlocksworldTask::get_axiom_effect_condition(int, int, int) const
{
    abort();
}

FactPair BlocksworldTask::get_axiom_effect(int, int) const
{
    abort();
}

std::string BlocksworldTask::get_operator_name(int index) const
{
    return operators[index].name;
}

int BlocksworldTask::get_num_operators() const
{
    return static_cast<int>(operators.size());
}

int BlocksworldTask::get_num_operator_preconditions(int index) const
{
    return static_cast<int>(operators[index].preconditions.size());
}

FactPair
BlocksworldTask::get_operator_precondition(int index, int fact_index) const
{
    return operators[index].preconditions[fact_index];
}

int BlocksworldTask::get_num_goals() const
{
    return static_cast<int>(goal_state.size());
}

FactPair BlocksworldTask::get_goal_fact(int index) const
{
    return goal_state[index];
}

std::vector<int> BlocksworldTask::get_initial_state_values() const
{
    return initial_state;
}

void BlocksworldTask::convert_ancestor_state_values(
    std::vector<int>&,
    const AbstractTaskBase*) const
{
    abort();
}

int BlocksworldTask::convert_operator_index(int, const AbstractTaskBase*) const
{
    abort();
}

value_t BlocksworldTask::get_operator_cost(int index) const
{
    return operators[index].cost;
}

int BlocksworldTask::get_num_operator_outcomes(int index) const
{
    return static_cast<int>(operators[index].outcomes.size());
}

value_t
BlocksworldTask::get_operator_outcome_probability(int index, int outcome_index)
    const
{
    return operators[index].outcomes[outcome_index].probability;
}

int BlocksworldTask::get_operator_outcome_id(int index, int outcome_index) const
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

int BlocksworldTask::get_num_operator_outcome_effects(
    int index,
    int outcome_index) const
{
    return static_cast<int>(
        operators[index].outcomes[outcome_index].effects.size());
}

FactPair BlocksworldTask::get_operator_outcome_effect(
    int index,
    int outcome_index,
    int eff_index) const
{
    return operators[index].outcomes[outcome_index].effects[eff_index];
}

int BlocksworldTask::get_num_operator_outcome_effect_conditions(int, int, int)
    const
{
    return 0;
}

FactPair
BlocksworldTask::get_operator_outcome_effect_condition(int, int, int, int) const
{
    abort();
}

int BlocksworldTask::get_clear_var(int block) const
{
    return block;
}
int BlocksworldTask::get_location_var(int block) const
{
    return blocks + block;
}
int BlocksworldTask::get_hand_var() const
{
    return 2 * blocks;
}

FactPair
BlocksworldTask::get_fact_is_block_clear(int block, bool is_clear) const
{
    return FactPair{get_clear_var(block), is_clear ? 1 : 0};
}

FactPair BlocksworldTask::get_fact_block_on_block(int block1, int block2) const
{
    return FactPair{get_location_var(block1), block2};
}

FactPair BlocksworldTask::get_fact_block_on_table(int block) const
{
    return FactPair{get_location_var(block), blocks};
}

FactPair BlocksworldTask::get_fact_block_in_hand(int block) const
{
    return FactPair{get_location_var(block), blocks + 1};
}

FactPair BlocksworldTask::get_fact_is_hand_empty(bool is_empty) const
{
    return FactPair{get_hand_var(), is_empty ? 1 : 0};
}

int BlocksworldTask::get_operator_pick_up_tower_index(int b1, int b2, int b3)
    const
{
    assert(b1 != b2 && b1 != b3 && b2 != b3);
    if (b3 >= b1) --b3;
    if (b3 >= b2) --b3;
    if (b2 >= b1) --b2;
    return b1 * (blocks - 1) * (blocks - 2) + b2 * (blocks - 2) + b3;
}

int BlocksworldTask::get_operator_pick_up_block_on_block_index(int b1, int b2)
    const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return pick_up_begin + b1 * (blocks - 1) + b2;
}

int BlocksworldTask::get_operator_pick_up_block_from_table_index(int b) const
{
    return pick_table_begin + b;
}

int BlocksworldTask::get_operator_put_tower_on_block_index(
    int b1,
    int b2,
    int b3) const
{
    assert(b1 != b2 && b1 != b3 && b2 != b3);
    if (b3 >= b1) --b3;
    if (b3 >= b2) --b3;
    if (b2 >= b1) --b2;
    return put_tower_block_begin + b1 * (blocks - 1) * (blocks - 2) +
           b2 * (blocks - 2) + b3;
}

int BlocksworldTask::get_operator_put_tower_on_table_index(int b1, int b2) const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return put_tower_down_begin + b1 * (blocks - 1) + b2;
}

int BlocksworldTask::get_operator_put_block_on_block_index(int b1, int b2) const
{
    assert(b1 != b2);
    if (b2 > b1) --b2;
    return put_on_block_begin + b1 * (blocks - 1) + b2;
}

int BlocksworldTask::get_operator_put_block_on_table_index(int b) const
{
    return put_table_begin + b;
}

State BlocksworldTask::get_state(const std::vector<FactPair>& facts)
{
    std::vector<int> values(variables.size(), -1);

    for (const auto& [var, val] : facts) {
        if (values[var] != -1) abort();
        values[var] = val;
    }

    for (const int val : values) {
        if (val == -1) abort();
    }

    return State(*this, std::move(values));
}

} // namespace tests
