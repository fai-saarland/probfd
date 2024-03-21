#include "probfd/merge_and_shrink/pruning_strategy_solvable.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation PruningStrategySolvable::compute_pruning_abstraction(
    const TransitionSystem& ts,
    const Distances& distances,
    utils::LogProxy& log)
{
    int num_states = ts.get_size();
    StateEquivalenceRelation state_equivalence_relation;
    state_equivalence_relation.reserve(num_states);
    int unsolvable = 0;
    for (int state = 0; state < num_states; ++state) {
        /* If pruning both unreachable and irrelevant states, a state which is
           dead is counted for both statistics! */
        assert(distances.are_goal_distances_computed());
        if (distances.get_goal_distance(state) != INFINITE_VALUE) {
            state_equivalence_relation.push_back({state});
        } else {
            ++unsolvable;
        }
    }

    if (log.is_at_least_verbose() && unsolvable > 0) {
        log << ts.tag() << "pruned: " << unsolvable << " unsolvable states"
            << std::endl;
    }

    return state_equivalence_relation;
}

} // namespace probfd::merge_and_shrink