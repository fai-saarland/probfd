#ifndef PROBFD_MERGE_AND_SHRINK_PRUNE_STRATEGY_H
#define PROBFD_MERGE_AND_SHRINK_PRUNE_STRATEGY_H

#include "probfd/merge_and_shrink/types.h"

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class Distances;
class TransitionSystem;
}

namespace probfd::merge_and_shrink {

/*
  A pruning strategy specifies which states from a factor are pruned in between
  merge steps.
*/
class PruneStrategy {
public:
    virtual ~PruneStrategy() = default;

    virtual StateEquivalenceRelation compute_pruning_abstraction(
        const TransitionSystem& ts,
        const Distances& distances,
        utils::LogProxy& log) = 0;

    virtual bool requires_liveness() const = 0;
    virtual bool requires_goal_distances() const = 0;

    virtual void dump_options(utils::LogProxy& log) const = 0;
};

} // namespace probfd::merge_and_shrink

#endif
