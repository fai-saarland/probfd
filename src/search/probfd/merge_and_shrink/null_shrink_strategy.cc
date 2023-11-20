#include "probfd/merge_and_shrink/null_shrink_strategy.h"

#include "probfd/merge_and_shrink/transition_system.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation NullShrinkStrategy::compute_equivalence_relation(
    const TransitionSystem& ts,
    const Distances&,
    int,
    utils::LogProxy&) const

{
    StateEquivalenceRelation relation;
    for (int index = 0; index != ts.get_size(); ++index) {
        relation.push_back({index});
    }
    return relation;
}

bool NullShrinkStrategy::requires_init_distances() const
{
    return false;
}

bool NullShrinkStrategy::requires_goal_distances() const
{
    return false;
}

std::string NullShrinkStrategy::name() const
{
    return "no shrinking";
}

void NullShrinkStrategy::dump_strategy_specific_options(utils::LogProxy&) const
{
    // Nothing to report.
}

} // namespace probfd::merge_and_shrink