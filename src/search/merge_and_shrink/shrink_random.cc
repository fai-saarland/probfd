#include "merge_and_shrink/shrink_random.h"

#include "merge_and_shrink/abstraction.h"

#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <memory>

using namespace std;

namespace merge_and_shrink {

ShrinkRandom::ShrinkRandom(const options::Options &opts)
    : ShrinkBucketBased(opts)
{
}

ShrinkRandom::~ShrinkRandom()
{
}

string ShrinkRandom::name() const
{
    return "random";
}

void ShrinkRandom::partition_into_buckets(
    const Abstraction &abs, vector<Bucket> &buckets) const
{
    assert(buckets.empty());
    buckets.resize(1);
    Bucket &big_bucket = buckets.back();
    big_bucket.reserve(abs.size());
    for (AbstractStateRef state = 0; state < abs.size(); ++state) {
        big_bucket.push_back(state);
    }
    assert(!big_bucket.empty());
}

static std::shared_ptr<ShrinkStrategy> _parse(options::OptionParser &parser)
{
    ShrinkStrategy::add_options_to_parser(parser);
    options::Options opts = parser.parse();

    if (!parser.dry_run()) {
        ShrinkStrategy::handle_option_defaults(opts);
        return std::make_shared< ShrinkRandom>(opts);
    } else {
        return 0;
    }
}

static Plugin<ShrinkStrategy> _plugin("shrink_random", _parse);

}
