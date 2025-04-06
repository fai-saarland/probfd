#include "downward/utils/rng_options.h"

#include "downward/utils/rng.h"

using namespace std;

namespace downward::utils {

shared_ptr<RandomNumberGenerator> get_rng(int seed)
{
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
