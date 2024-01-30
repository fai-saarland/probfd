#ifndef PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_EQUAL_DISTANCE_H
#define PROBFD_MERGE_AND_SHRINK_SHRINK_STRATEGY_EQUAL_DISTANCE_H

#include "probfd/merge_and_shrink/shrink_strategy_bucket_based.h"

#include <vector>

namespace downward::cli::plugins {
class Options;
}

namespace probfd::merge_and_shrink {

/*
  NOTE: In case where we must merge across buckets (i.e. when
  the number of (f, h) pairs is larger than the number of
  permitted abstract states), this shrink strategy will *not* make
  an effort to be at least be h-preserving.

  This could be improved, but not without complicating the code.
  Usually we set the number of abstract states large enough that we
  do not need to merge across buckets. Therefore the complication
  might not be worth the code maintenance cost.
*/
class ShrinkStrategyEqualDistance : public ShrinkStrategyBucketBased {
public:
    enum class Priority { HIGH, LOW };

private:
    const Priority h_start;

public:
    ShrinkStrategyEqualDistance(int random_seed, Priority high_low);

    bool requires_liveness() const override { return false; }
    bool requires_goal_distances() const override { return true; }

protected:
    std::string name() const override;

    void dump_strategy_specific_options(utils::LogProxy& log) const override;

    std::vector<Bucket> partition_into_buckets(
        const TransitionSystem& ts,
        const Distances& distances) const override;

private:
    std::vector<Bucket> ordered_buckets_use_vector(
        const TransitionSystem& ts,
        const Distances& distances,
        int max_h) const;

    std::vector<Bucket> ordered_buckets_use_map(
        const TransitionSystem& ts,
        const Distances& distances) const;
};

} // namespace probfd::merge_and_shrink

#endif
