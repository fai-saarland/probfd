#include "probfd/merge_and_shrink/shrink_h.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/value_type.h"

#include "downward/plugins/plugin.h"
#include "downward/utils/collections.h"
#include "downward/utils/logging.h"
#include "downward/utils/markup.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <vector>

using namespace std;

namespace probfd::merge_and_shrink {

ShrinkH::ShrinkH(const plugins::Options& opts)
    : ShrinkBucketBased(opts)
    , h_start(opts.get<HighLow>("shrink_h"))
{
}

vector<ShrinkBucketBased::Bucket> ShrinkH::partition_into_buckets(
    const TransitionSystem& ts,
    const Distances& distances) const
{
    assert(distances.are_init_distances_computed());
    assert(distances.are_goal_distances_computed());

    return ordered_buckets_use_map(ts, distances);
}

vector<ShrinkBucketBased::Bucket> ShrinkH::ordered_buckets_use_map(
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
            [](double x, double y) { return probfd::is_approx_less(x, y); },
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

    if (h_start == ShrinkH::HighLow::HIGH) {
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

string ShrinkH::name() const
{
    return "h-preserving";
}

void ShrinkH::dump_strategy_specific_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Prefer shrinking high or low h states: "
            << (h_start == HighLow::HIGH ? "high" : "low") << endl;
    }
}

class ShrinkHFeature : public plugins::TypedFeature<ShrinkStrategy, ShrinkH> {
public:
    ShrinkHFeature()
        : TypedFeature("shrink_h")
    {
        document_title("h-preserving shrink strategy");

        ShrinkBucketBased::add_options_to_feature(*this);
        add_option<ShrinkH::HighLow>(
            "shrink_h",
            "in which direction the h based shrink priority is ordered",
            "low");

        document_note(
            "Note",
            "The strategy first partitions all states according to their "
            "h-values. States sorted last are shrinked together until reaching "
            "max_states.");
    }
};

static plugins::FeaturePlugin<ShrinkHFeature> _plugin;

static plugins::TypedEnumPlugin<ShrinkH::HighLow> _enum_plugin(
    {{"high", "prefer shrinking states with high value"},
     {"low", "prefer shrinking states with low value"}});

} // namespace probfd