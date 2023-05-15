#include "tasks/blocksworld.h"

#include <set>

namespace probfd {
namespace tests {

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

BlocksworldTask::BlocksworldTask(
    int num_blocks,
    const std::vector<std::vector<int>>& initial,
    const std::vector<std::vector<int>>& goal)
    : blocks(num_blocks)
    , initial_state(2 * blocks + 1, 0)
    , pick_tower_begin(0)
    , pick_up_begin(pick_tower_begin + blocks * blocks * blocks)
    , pick_table_begin(pick_up_begin + blocks * blocks)
    , put_tower_block_begin(pick_table_begin + blocks)
    , put_tower_down_begin(
          put_tower_block_begin + blocks * blocks * (blocks - 1))
    , put_on_block_begin(put_tower_down_begin + blocks * blocks)
    , put_table_begin(put_on_block_begin + blocks * (blocks - 1))
    , num_operators(put_table_begin + blocks)
{
    /*
     * Read the initial state
     */

    for (int i = 0; i != blocks; ++i) {
        // Assume all blocks are clear and in the hand in the beginning
        initial_state[get_clear_var(i)] = 1;
        initial_state[get_location_var(i)] = blocks + 1;
        initial_state[get_hand_var()] = 0;
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

    if (seen.size() == blocks) {
        // All blocks placed, hand is empty
        initial_state[get_hand_var()] = 1;
    } else if (seen.size() != blocks - 1) {
        // Only one block may be held
        abort();
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
}

int BlocksworldTask::get_num_variables() const
{
    return 2 * blocks + 1;
}

std::string BlocksworldTask::get_variable_name(int var) const
{
    return var < blocks       ? "clear-status" + std::to_string(var)
           : var < 2 * blocks ? "location"
                              : "hand";
}

int BlocksworldTask::get_variable_domain_size(int var) const
{
    return var < blocks ? 2 : var < 2 * blocks ? blocks + 2 : 2;
}

int BlocksworldTask::get_variable_axiom_layer(int var) const
{
    return -1;
}

int BlocksworldTask::get_variable_default_axiom_value(int var) const
{
    return 0;
}

std::string BlocksworldTask::get_fact_name(const FactPair& fact) const
{
    if (fact.var < blocks) {
        return std::string("block #") + std::to_string(fact.var) +
               std::string(" ") +
               (fact.value == 0 ? std::string("not clear")
                                : std::string("clear"));
    } else if (fact.var < 2 * blocks) {
        if (fact.value < blocks) {
            return std::string("block #") + std::to_string(fact.var) +
                   " on block #" + std::to_string(fact.value);
        } else if (fact.value == blocks) {
            return std::string("block #") + std::to_string(fact.var) +
                   " on table";
        } else {
            return std::string("block #") + std::to_string(fact.var) +
                   " in hand";
        }
    } else {
        return std::string("Hand is ") +
               (fact.value == 0 ? "not empty" : "empty");
    }
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
    if (index < pick_up_begin) {
        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % blocks;

        return "pick-tower " + std::to_string(b1) + " " + std::to_string(b2) +
               " " + std::to_string(b3);
    } else if (index < pick_table_begin) {
        index -= pick_up_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        return "pick-up " + std::to_string(b1) + " " + std::to_string(b2);
    } else if (index < put_tower_block_begin) {
        index -= pick_table_begin;

        int b1 = index % blocks;

        return "pick-up-from-table " + std::to_string(b1);
    } else if (index < put_tower_down_begin) {
        index -= put_tower_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % (blocks - 1);

        if (b3 >= b1) ++b3;

        return "put-tower-on-block " + std::to_string(b1) + " " +
               std::to_string(b2) + " " + std::to_string(b3);
    } else if (index < put_on_block_begin) {
        index -= put_tower_down_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        return "put-tower-down " + std::to_string(b1) + " " +
               std::to_string(b2);
    } else if (index < put_table_begin) {
        index -= put_on_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % (blocks - 1);

        if (b2 >= b1) ++b2;

        return "put-on-block " + std::to_string(b1) + " " + std::to_string(b2);
    } else {
        index -= put_table_begin;

        int b1 = index % blocks;
        return "put-down " + std::to_string(b1);
    }
}

int BlocksworldTask::get_num_operators() const
{
    return num_operators;
}

int BlocksworldTask::get_num_operator_preconditions(int index) const
{
    if (index < pick_up_begin) {
        return 4;
    } else if (index < pick_table_begin) {
        return 3;
    } else if (index < put_tower_block_begin) {
        return 3;
    } else if (index < put_tower_down_begin) {
        return 3;
    } else if (index < put_on_block_begin) {
        return 2;
    } else if (index < put_table_begin) {
        return 3;
    } else {
        return 2;
    }
}

FactPair
BlocksworldTask::get_operator_precondition(int index, int fact_index) const
{
    if (index < pick_up_begin) {
        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % blocks;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_hand_var(), 1);
        case 1: return FactPair(get_clear_var(b1), 1);
        case 2: return FactPair(get_location_var(b1), b2);
        case 3: return FactPair(get_location_var(b2), b3);
        }
    } else if (index < pick_table_begin) {
        index -= pick_up_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_hand_var(), 1);
        case 1: return FactPair(get_clear_var(b1), 1);
        case 2: return FactPair(get_location_var(b1), b2);
        }
    } else if (index < put_tower_block_begin) {
        index -= pick_table_begin;

        int b = index % blocks;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_hand_var(), 1);
        case 1: return FactPair(get_clear_var(b), 1);
        case 2: return FactPair(get_location_var(b), blocks);
        }
    } else if (index < put_tower_down_begin) {
        index -= put_tower_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % (blocks - 1);

        if (b3 >= b1) ++b3;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b2), blocks + 1);
        case 1: return FactPair(get_location_var(b1), b2);
        case 2: return FactPair(get_clear_var(b3), 1);
        }
    } else if (index < put_on_block_begin) {
        index -= put_tower_down_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b2), blocks + 1);
        case 1: return FactPair(get_location_var(b1), b2);
        }
    } else if (index < put_table_begin) {
        index -= put_on_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % (blocks - 1);

        if (b2 >= b1) ++b2;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b1), blocks + 1);
        case 1: return FactPair(get_clear_var(b1), 1);
        case 2: return FactPair(get_clear_var(b2), 1);
        }
    } else {
        index -= put_table_begin;

        int b = index % blocks;

        switch (fact_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b), blocks + 1);
        case 1: return FactPair(get_clear_var(b), 1);
        }
    }
}

int BlocksworldTask::get_num_goals() const
{
    return goal_state.size();
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
    if (index < pick_up_begin) {
        return 0_vt;
    } else if (index < pick_table_begin) {
        return 1_vt;
    } else if (index < put_tower_block_begin) {
        return 1_vt;
    } else if (index < put_tower_down_begin) {
        return 0_vt;
    } else if (index < put_on_block_begin) {
        return 0_vt;
    } else if (index < put_table_begin) {
        return 0_vt;
    } else {
        return 0_vt;
    }
}

int BlocksworldTask::get_num_operator_outcomes(int index) const
{
    if (index < pick_up_begin) {
        return 2;
    } else if (index < pick_table_begin) {
        return 2;
    } else if (index < put_tower_block_begin) {
        return 2;
    } else if (index < put_tower_down_begin) {
        return 2;
    } else if (index < put_on_block_begin) {
        return 1;
    } else if (index < put_table_begin) {
        return 2;
    } else {
        return 1;
    }
}

value_t
BlocksworldTask::get_operator_outcome_probability(int index, int outcome_index)
    const
{
    if (index < pick_up_begin) {
        return outcome_index == 0 ? 0.1_vt : 0.9_vt;
    } else if (index < pick_table_begin) {
        return outcome_index == 0 ? 0.75_vt : 0.25_vt;
    } else if (index < put_tower_block_begin) {
        return outcome_index == 0 ? 0.75_vt : 0.25_vt;
    } else if (index < put_tower_down_begin) {
        return outcome_index == 0 ? 0.1_vt : 0.9_vt;
    } else if (index < put_on_block_begin) {
        return 1_vt;
    } else if (index < put_table_begin) {
        return outcome_index == 0 ? 0.75_vt : 0.25_vt;
    } else {
        return 1_vt;
    }
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
    if (index < pick_up_begin) {
        return outcome_index == 0 ? 3 : 0;
    } else if (index < pick_table_begin) {
        return outcome_index == 0 ? 3 : 2;
    } else if (index < put_tower_block_begin) {
        return outcome_index == 0 ? 2 : 0;
    } else if (index < put_tower_down_begin) {
        return outcome_index == 0 ? 3 : 2;
    } else if (index < put_on_block_begin) {
        return 2;
    } else if (index < put_table_begin) {
        return outcome_index == 0 ? 4 : 3;
    } else {
        return 3;
    }
}

FactPair BlocksworldTask::get_operator_outcome_effect(
    int index,
    int outcome_index,
    int eff_index) const
{
    if (index < pick_up_begin) {
        if (outcome_index == 1) abort();

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % blocks;

        switch (eff_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b2), blocks + 1);
        case 1: return FactPair(get_clear_var(b3), 1);
        case 2: return FactPair(get_hand_var(), 0);
        }
    } else if (index < pick_table_begin) {
        index -= pick_up_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        if (outcome_index == 0) {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b1), blocks + 1);
            case 1: return FactPair(get_clear_var(b2), 1);
            case 2: return FactPair(get_hand_var(), 0);
            }
        } else {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b1), blocks);
            case 1: return FactPair(get_clear_var(b2), 1);
            }
        }
    } else if (index < put_tower_block_begin) {
        index -= pick_table_begin;

        int b = index % blocks;

        switch (eff_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b), blocks + 1);
        case 1: return FactPair(get_hand_var(), 0);
        }
    } else if (index < put_tower_down_begin) {
        index -= put_tower_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;
        int b3 = (index / (blocks * blocks)) % (blocks - 1);

        if (b3 >= b1) ++b3;

        if (outcome_index == 0) {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b2), b3);
            case 1: return FactPair(get_hand_var(), 1);
            case 2: return FactPair(get_clear_var(b3), 0);
            }
        } else {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b2), blocks);
            case 1: return FactPair(get_hand_var(), 1);
            }
        }
    } else if (index < put_on_block_begin) {
        index -= put_tower_down_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % blocks;

        switch (eff_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b2), blocks);
        case 1: return FactPair(get_hand_var(), 1);
        }
    } else if (index < put_table_begin) {
        index -= put_on_block_begin;

        int b1 = index % blocks;
        int b2 = (index / blocks) % (blocks - 1);

        if (b2 >= b1) ++b2;

        if (outcome_index == 0) {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b1), b2);
            case 1: return FactPair(get_hand_var(), 1);
            case 2: return FactPair(get_clear_var(b1), 1);
            case 3: return FactPair(get_clear_var(b2), 0);
            }
        } else {
            switch (eff_index) {
            default: abort();
            case 0: return FactPair(get_location_var(b1), blocks);
            case 1: return FactPair(get_hand_var(), 1);
            case 2: return FactPair(get_clear_var(b1), 1);
            }
        }
    } else {
        index -= put_table_begin;

        int b = index % blocks;

        switch (eff_index) {
        default: abort();
        case 0: return FactPair(get_location_var(b), blocks);
        case 1: return FactPair(get_hand_var(), 1);
        case 2: return FactPair(get_clear_var(b), 1);
        }
    }
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

} // namespace tests
} // namespace probfd