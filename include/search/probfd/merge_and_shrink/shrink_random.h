#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_RANDOM_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_RANDOM_H

#include "probfd/merge_and_shrink/shrink_bucket_based.h"

namespace plugins {
class Options;
}

namespace probfd::merge_and_shrink {

class ShrinkRandom : public ShrinkBucketBased {
public:
    explicit ShrinkRandom(const plugins::Options& opts);

    virtual bool requires_liveness() const override { return false; }
    virtual bool requires_goal_distances() const override { return false; }

protected:
    virtual std::vector<Bucket> partition_into_buckets(
        const TransitionSystem& ts,
        const Distances& distances) const override;

    virtual std::string name() const override;
    void dump_strategy_specific_options(utils::LogProxy&) const override {}
};

} // namespace probfd::merge_and_shrink

#endif // PROBFD_HEURISTICS_MERGE_AND_SHRINK_SHRINK_RANDOM_H
