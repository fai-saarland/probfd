#include "downward/utils/rng_options.h"

#include "downward/utils/rng.h"

#include "downward/plugins/plugin.h"

using namespace std;

namespace utils {
void add_rng_options(plugins::Feature& feature)
{
    feature.add_option<int>(
        "random_seed",
        "Set to -1 (default) to use the global random number generator. "
        "Set to any other value to use a local random number generator with "
        "the given seed.",
        "-1",
        plugins::Bounds("-1", "infinity"));
}

shared_ptr<RandomNumberGenerator>
parse_rng_from_options(const plugins::Options& options)
{
    int seed = options.get<int>("random_seed");
    if (seed == -1) {
        // Use an arbitrary default seed.
        static shared_ptr<utils::RandomNumberGenerator> rng =
            make_shared<utils::RandomNumberGenerator>(2011);
        return rng;
    } else {
        return make_shared<RandomNumberGenerator>(seed);
    }
}
} // namespace utils
