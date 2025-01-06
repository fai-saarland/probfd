#include "downward/merge_and_shrink/shrink_random.h"

#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/transition_system.h"

#include <cassert>

using namespace std;

namespace merge_and_shrink {
ShrinkRandom::ShrinkRandom(int random_seed)
    : ShrinkBucketBased(random_seed)
{
}

vector<ShrinkBucketBased::Bucket> ShrinkRandom::partition_into_buckets(
    const TransitionSystem& ts,
    const Distances&) const
{
    vector<Bucket> buckets;
    buckets.resize(1);
    Bucket& big_bucket = buckets.back();
    big_bucket.reserve(ts.get_size());
    int num_states = ts.get_size();
    for (int state = 0; state < num_states; ++state)
        big_bucket.push_back(state);
    assert(!big_bucket.empty());
    return buckets;
}

string ShrinkRandom::name() const
{
    return "random";
}

} // namespace merge_and_shrink
