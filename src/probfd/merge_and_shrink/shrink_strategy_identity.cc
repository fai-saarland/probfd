#include "probfd/merge_and_shrink/shrink_strategy_identity.h"

#include "probfd/merge_and_shrink/transition_system.h"

namespace probfd::merge_and_shrink {

StateEquivalenceRelation ShrinkStrategyIdentity::compute_equivalence_relation(
    const TransitionSystem& ts,
    const Distances&,
    int,
    utils::LogProxy&) const

{
    return {
        std::from_range,
        std::views::iota(0, ts.get_size()) |
            std::views::transform(
                [](auto index) -> StateEquivalenceClass { return {index}; })};
}

bool ShrinkStrategyIdentity::requires_init_distances() const
{
    return false;
}

bool ShrinkStrategyIdentity::requires_goal_distances() const
{
    return false;
}

std::string ShrinkStrategyIdentity::name() const
{
    return "no shrinking";
}

void ShrinkStrategyIdentity::dump_strategy_specific_options(
    utils::LogProxy&) const
{
    // Nothing to report.
}

} // namespace probfd::merge_and_shrink