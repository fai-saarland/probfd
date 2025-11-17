#include "probfd/cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

#include "downward/cli/utils/rng_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::cli::merge_and_shrink {

std::size_t add_bucket_based_shrink_options_to_feature(
    Feature& feature,
    std::size_t start_index)
{
    return downward::cli::utils::add_rng_options_to_feature(
        feature,
        start_index);
}

} // namespace probfd::cli::merge_and_shrink