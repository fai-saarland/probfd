#include "downward/utils/rng_options.h"

#include "downward/utils/rng.h"

using namespace std;

namespace downward::utils {

shared_ptr<RandomNumberGenerator> get_default_rng()
{
    // Use an arbitrary default seed.
    static auto rng = make_shared<RandomNumberGenerator>(2011);
    return rng;
}

shared_ptr<RandomNumberGenerator> get_rng(int seed)
{
    if (seed == -1) {
        return get_default_rng();
    }

    return make_shared<RandomNumberGenerator>(seed);
}
} // namespace downward::utils
