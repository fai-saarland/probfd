#include "probfd/merge_and_shrink/shrink_strategy_equal_distance.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/value_type.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"

#include "downward/cli/plugins/plugin.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <ranges>
#include <vector>

using namespace std;

using namespace downward::cli::plugins;

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
        for (auto& bucket : views::reverse(views::values(states_by_h))) {
            buckets.emplace_back(std::move(bucket));
        }
    } else {
        for (auto& bucket : views::values(states_by_h)) {
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

namespace {

class ShrinkStrategyEqualDistanceFeature
    : public TypedFeature<ShrinkStrategy, ShrinkStrategyEqualDistance> {
public:
    ShrinkStrategyEqualDistanceFeature()
        : TypedFeature("shrink_equal_distance")
    {
        document_title("distance-preserving shrink strategy");

        add_bucket_based_shrink_options_to_feature(*this);

        add_option<ShrinkStrategyEqualDistance::Priority>(
            "priority",
            "in which direction the distance based shrink priority is ordered",
            "low");

        document_note(
            "Note",
            "The strategy first partitions all states according to their "
            "h-values. States sorted last are shrinked together until reaching "
            "max_states.");
    }

protected:
    shared_ptr<ShrinkStrategyEqualDistance>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyEqualDistance>(
            get_bucket_based_shrink_args_from_options(options),
            options.get<ShrinkStrategyEqualDistance::Priority>("priority"));
    }
};

FeaturePlugin<ShrinkStrategyEqualDistanceFeature> _plugin;

TypedEnumPlugin<ShrinkStrategyEqualDistance::Priority> _enum_plugin(
    {{"high", "prefer shrinking states with high value"},
     {"low", "prefer shrinking states with low value"}});

} // namespace

} // namespace probfd::merge_and_shrink