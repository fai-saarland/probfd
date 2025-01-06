#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

using namespace std;

namespace downward::cli::merge_and_shrink {

void add_shrink_bucket_options_to_feature(plugins::Feature& feature)
{
    utils::add_rng_options_to_feature(feature);
}

tuple<int>
get_shrink_bucket_arguments_from_options(const plugins::Options& opts)
{
    return utils::get_rng_arguments_from_options(opts);
}

} // namespace downward::cli::merge_and_shrink
