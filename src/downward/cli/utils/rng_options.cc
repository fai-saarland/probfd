#include "downward/cli/utils/rng_options.h"

#include "downward/cli/plugins/plugin.h"

using namespace std;

namespace downward::cli::utils {

std::size_t
add_rng_options_to_feature(plugins::Feature& feature, std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "random_seed",
        "-1",
        "Set to -1 (default) to use the global random number generator. "
        "Set to any other value to use a local random number generator with "
        "the given seed.");
    return 1;
}

} // namespace downward::cli::utils
