#include "probfd/merge_and_shrink/shrink_strategy_random.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include <ranges>

using namespace std;

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

} // namespace probfd::merge_and_shrink