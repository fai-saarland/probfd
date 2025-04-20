#include "probfd/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <ranges>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

ShrinkStrategyEqualDistance::ShrinkStrategyEqualDistance(
    int random_seed,
    Priority high_low)
    : ShrinkStrategyBucketBased(random_seed)
    , h_start(high_low)
{
}

vector<ShrinkStrategyBucketBased::Bucket>
ShrinkStrategyEqualDistance::partition_into_buckets(
    const TransitionSystem& ts,
    const Distances& distances) const
{
    assert(distances.is_liveness_computed());
    assert(distances.are_goal_distances_computed());

    return ordered_buckets_use_map(ts, distances);
}

vector<ShrinkStrategyBucketBased::Bucket>
ShrinkStrategyEqualDistance::ordered_buckets_use_map(
    const TransitionSystem& ts,
    const Distances& distances) const
{
    map<value_t, Bucket> states_by_h;

    for (int state = 0; state < ts.get_size(); ++state) {
        value_t h = distances.get_goal_distance(state);

        // Get epsilon-close bucket, if existent
        auto [first, last] = std::ranges::equal_range(
            states_by_h,
            h,
            [](double x, double y) {
                return probfd::is_approx_less(x, y, 0.001);
            },
            [](const auto& p) { return p.first; });

        // If there are no entries, make a new bucket, otherwise add to the
        // epsilon close bucket. There cannot be more than one epsilon close
        // bucket.
        if (first == last) {
            states_by_h.emplace(h, state);
        } else {
            assert(std::distance(first, last) == 1);
            first->second.push_back(state);
        }
    }

    vector<Bucket> buckets;
    buckets.reserve(states_by_h.size());

    if (h_start == ShrinkStrategyEqualDistance::Priority::HIGH) {
        for (auto& bucket :
             std::views::reverse(std::views::values(states_by_h))) {
            buckets.emplace_back(std::move(bucket));
        }
    } else {
        for (auto& bucket : std::views::values(states_by_h)) {
            buckets.emplace_back(std::move(bucket));
        }
    }

    return buckets;
}

string ShrinkStrategyEqualDistance::name() const
{
    return "h-preserving";
}

void ShrinkStrategyEqualDistance::dump_strategy_specific_options(
    utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Prefer shrinking high or low h states: "
            << (h_start == Priority::HIGH ? "high" : "low") << endl;
    }
}

} // namespace probfd::merge_and_shrink