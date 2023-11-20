#include "probfd/merge_and_shrink/prune_strategy_identity.h"

#include "probfd/merge_and_shrink/transition_system.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation PruneStrategyIdentity::compute_pruning_abstraction(
    const TransitionSystem& ts,
    const Distances&,
    utils::LogProxy&)

{
    StateEquivalenceRelation relation;
    for (int index = 0; index != ts.get_size(); ++index) {
        relation.push_back({index});
    }
    return relation;
}

bool PruneStrategyIdentity::requires_init_distances() const
{
    return false;
}

bool PruneStrategyIdentity::requires_goal_distances() const
{
    return false;
}

void PruneStrategyIdentity::dump_options(utils::LogProxy&) const
{
    // Nothing to report.
}

} // namespace probfd::merge_and_shrink