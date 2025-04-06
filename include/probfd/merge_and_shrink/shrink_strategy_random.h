#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_RANDOM_H

#include "probfd/merge_and_shrink/shrink_strategy_bucket_based.h"

namespace probfd::merge_and_shrink {

class ShrinkStrategyRandom : public ShrinkStrategyBucketBased {
public:
    explicit ShrinkStrategyRandom(int random_seed);

    bool requires_liveness() const override { return false; }

    bool requires_goal_distances() const override { return false; }

protected:
    std::vector<Bucket> partition_into_buckets(
        const TransitionSystem& ts,
        const Distances& distances) const override;

    std::string name() const override;

    void
    dump_strategy_specific_options(downward::utils::LogProxy&) const override
    {
    }
};

} // namespace probfd::merge_and_shrink

#endif
