#include "probfd/heuristics/abstractions/saturation.h"

#include "probfd/mdp.h"

namespace probfd::heuristics::abstractions {

template <typename Action>
void compute_saturated_costs(
    AbstractMDP<Action>& mdp,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs)
{
    std::fill(saturated_costs.begin(), saturated_costs.end(), -INFINITE_VALUE);

    const int num_states = value_table.size();
    for (AbstractStateIndex s = 0; s < num_states; ++s) {
        const value_t value = value_table[s];

        if (value == INFINITE_VALUE) {
            // Skip dead ends and unreachable states
            continue;
        }

        // Generate operators...
        std::vector<Action> aops;
        mdp.generate_applicable_actions(s, aops);

        for (const Action op : aops) {
            int oid = op->operator_id.get_index();

            Distribution<StateID> successor_dist;
            mdp.generate_action_transitions(s, op, successor_dist);

            value_t h_succ = 0;

            for (const auto& [t, prob] : successor_dist) {
                const auto succ_val = value_table[t];

                if (succ_val == INFINITE_VALUE) {
                    // No need to consider dead transitions.
                    goto next_operator;
                }

                h_succ += prob * succ_val;
            }

            saturated_costs[oid] =
                std::max(saturated_costs[oid], value - h_succ);

        next_operator:;
        }
    }
}

} // namespace probfd::heuristics::abstractions
