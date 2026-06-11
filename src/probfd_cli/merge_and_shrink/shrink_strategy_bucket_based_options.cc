#include "probfd_cli/merge_and_shrink/shrink_strategy_bucket_based_options.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward_cli/utils/rng_options.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

using namespace language;
using namespace language::plugins;

namespace probfd::cli::merge_and_shrink {

void add_bucket_based_shrink_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_rng_options_to_feature(feature);
}

tuple<int> get_bucket_based_shrink_args_from_options(
    const Context& context,
    const Options& options)
{
    return downward::cli::utils::get_rng_arguments_from_options(
        context,
        options);
}

} // namespace probfd::cli::merge_and_shrink