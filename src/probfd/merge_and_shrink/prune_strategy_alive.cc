#include "probfd/merge_and_shrink/prune_strategy_alive.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"

using namespace downward;

namespace probfd::merge_and_shrink {

StateEquivalenceRelation PruneStrategyAlive::compute_pruning_abstraction(
    const TransitionSystem& ts,
    const Distances& distances,
    utils::LogProxy& log)
{
    const int num_states = ts.get_size();
    StateEquivalenceRelation state_equivalence_relation;
    state_equivalence_relation.reserve(num_states);
    int non_alive = 0;
    for (int state = 0; state < num_states; ++state) {
        /* If pruning both unreachable and irrelevant states, a state which is
           dead is counted for both statistics! */
        assert(distances.is_liveness_computed());
        if (distances.is_alive(state)) {
            state_equivalence_relation.push_back({state});
        } else {
            ++non_alive;
        }
    }

    if (log.is_at_least_verbose() && non_alive > 0) {
        log.print(ts.tag());
        log.println("pruned: {} non-alive states", non_alive);
    }

    return state_equivalence_relation;
}

bool PruneStrategyAlive::requires_liveness() const
{
    return true;
}

bool PruneStrategyAlive::requires_goal_distances() const
{
    return true;
}

} // namespace probfd::merge_and_shrink