#ifndef POTENTIALS_UTIL_H
#define POTENTIALS_UTIL_H

#include <vector>

namespace downward {
class State;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::potentials {
class PotentialOptimizer;

std::vector<State> sample_without_dead_end_detection(
    PotentialOptimizer& optimizer,
    int num_samples,
    utils::RandomNumberGenerator& rng);

} // namespace potentials

#endif
