#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_NULL_SHRINK_STRATEGY_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_NULL_SHRINK_STRATEGY_H

#include "probfd/merge_and_shrink/shrink_strategy.h"

namespace probfd::merge_and_shrink {

/*
  A pruning strategy specifies which states from a factor are pruned in between
  merge steps.
*/
class NullShrinkStrategy : public ShrinkStrategy {
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

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_NULL_SHRINK_STRATEGY_H
