#include "downward/cli/utils/rng_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;

namespace downward::cli::utils {

void add_rng_options_to_feature(plugins::Feature& feature)
{
    feature.add_optional_argument_with_default<int>(
        "random_seed",
        "-1",
        "Set to -1 (default) to use the global random number generator. "
        "Set to any other value to use a local random number generator with "
        "the given seed.");
}

tuple<int> get_rng_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<int>("random_seed"));
}

} // namespace downward::cli::utils
