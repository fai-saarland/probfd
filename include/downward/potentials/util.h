#ifndef POTENTIALS_UTIL_H
#define POTENTIALS_UTIL_H

#include <vector>

class State;

namespace utils {
class RandomNumberGenerator;
}

namespace potentials {
class PotentialOptimizer;

std::vector<State> sample_without_dead_end_detection(
    PotentialOptimizer& optimizer,
    int num_samples,
    utils::RandomNumberGenerator& rng);

} // namespace potentials

#endif
