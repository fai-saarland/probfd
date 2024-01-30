#include "probfd/merge_and_shrink/null_pruning_strategy.h"

#include "probfd/merge_and_shrink/transition_system.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation NullPruningStrategy::compute_pruning_abstraction(
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

bool NullPruningStrategy::requires_liveness() const
{
    return false;
}

bool NullPruningStrategy::requires_goal_distances() const
{
    return false;
}

void NullPruningStrategy::dump_options(utils::LogProxy&) const
{
    // Nothing to report.
}

} // namespace probfd::merge_and_shrink