#ifndef PROBFD_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H
#define PROBFD_MERGE_AND_SHRINK_PRUNE_STRATEGY_IDENTITY_H

#include "probfd/merge_and_shrink/prune_strategy.h"

namespace probfd::merge_and_shrink {

class PruneStrategyIdentity : public PruneStrategy {
public:
    StateEquivalenceRelation compute_pruning_abstraction(
        const TransitionSystem& ts,
        const Distances& distances,
        utils::LogProxy& log) override;

    bool requires_liveness() const override;
    bool requires_goal_distances() const override;

    void dump_options(utils::LogProxy& log) const override;
};

} // namespace probfd::merge_and_shrink

#endif
