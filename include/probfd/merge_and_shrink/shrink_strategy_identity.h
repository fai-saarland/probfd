#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_IDENTITY_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_IDENTITY_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace probfd::merge_and_shrink {

/*
  A pruning strategy specifies which states from a factor are pruned in between
  merge steps.
*/
class ShrinkStrategyIdentity : public ShrinkStrategy {
public:
    StateEquivalenceRelation compute_equivalence_relation(
        const TransitionSystem& ts,
        const Distances& distances,
        int target_size,
        utils::LogProxy& log) const override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

protected:
    std::string name() const override;

    void dump_strategy_specific_options(utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
