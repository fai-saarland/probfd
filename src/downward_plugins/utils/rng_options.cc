#include "downward_plugins/utils/rng_options.h"

#include "downward_plugins/plugins/plugin.h"

using namespace std;

namespace downward_plugins::utils {

void add_rng_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<int>(
        "random_seed",
        "Set to -1 (default) to use the global random number generator. "
        "Set to any other value to use a local random number generator with "
        "the given seed.",
        "-1",
        plugins::Bounds("-1", "infinity"));
}

tuple<int> get_rng_arguments_from_options(const plugins::Options& opts)
{
    return make_tuple(opts.get<int>("random_seed"));
}

} // namespace downward_plugins::utils
