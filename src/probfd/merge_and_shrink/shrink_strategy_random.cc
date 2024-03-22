#include "probfd/merge_and_shrink/shrink_strategy_random.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/cli/plugins/plugin.h"

#include <ranges>

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

ShrinkStrategyRandom::ShrinkStrategyRandom(int random_seed)
    : ShrinkStrategyBucketBased(random_seed)
{
}

vector<ShrinkStrategyBucketBased::Bucket>
ShrinkStrategyRandom::partition_into_buckets(
    const TransitionSystem& ts,
    const Distances&) const
{
    return {std::views::iota(0, ts.get_size()) | std::ranges::to<Bucket>()};
}

string ShrinkStrategyRandom::name() const
{
    return "random";
}

namespace {

class ShrinkRandomFeature
    : public TypedFeature<ShrinkStrategy, ShrinkStrategyRandom> {
public:
    ShrinkRandomFeature()
        : TypedFeature("pshrink_random")
    {
        document_title("Random Shrink Strategy");
        document_synopsis(
            "This strategy picks states to shrink uniformly at random.");

        add_bucket_based_shrink_options_to_feature(*this);
    }

protected:
    shared_ptr<ShrinkStrategyRandom>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ShrinkStrategyRandom>(
            get_bucket_based_shrink_args_from_options(options));
    }
};

FeaturePlugin<ShrinkRandomFeature> _plugin;

} // namespace

} // namespace probfd::merge_and_shrink