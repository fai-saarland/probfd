#include "probfd/merge_and_shrink/shrink_strategy_bucket_based.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/cli/utils/rng_options.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace std;
using namespace downward::cli::plugins;
using namespace probfd::merge_and_shrink;

namespace probfd::cli::merge_and_shrink {

void add_bucket_based_shrink_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_rng_options_to_feature(feature);
}

tuple<int> get_bucket_based_shrink_args_from_options(const Options& options)
{
    return downward::cli::utils::get_rng_arguments_from_options(options);
}

} // namespace probfd::merge_and_shrink