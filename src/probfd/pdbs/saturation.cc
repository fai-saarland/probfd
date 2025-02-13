#include "probfd/pdbs/saturation.h"

#include "probfd/pdbs/projection_operator.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/transition_tail.h"

#include <vector>

namespace probfd::pdbs {

void compute_saturated_costs(
    ProjectionStateSpace& state_space,
    std::span<const value_t> value_table,
    std::span<value_t> saturated_costs)
{
    std::fill(saturated_costs.begin(), saturated_costs.end(), -INFINITE_VALUE);

    const int num_states = value_table.size();
    for (StateRank s = 0; s < num_states; ++s) {
        const value_t value = value_table[s];

        if (value == INFINITE_VALUE) {
            // Skip dead ends and unreachable states
            continue;
        }

        // Generate operators...
        std::vector<const ProjectionOperator*> aops;
        state_space.generate_applicable_actions(s, aops);

        for (const ProjectionOperator* op : aops) {
            int oid = op->operator_id.get_index();

            SuccessorDistribution successor_dist;
            state_space.generate_action_transitions(s, op, successor_dist);

            value_t h_succ = 0;

            for (const auto& [t, prob] :
                 successor_dist.non_source_successor_dist) {
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

} // namespace probfd::pdbs