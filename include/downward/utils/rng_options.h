#ifndef UTILS_RNG_OPTIONS_H
#define UTILS_RNG_OPTIONS_H

#include <memory>

namespace downward::utils {

class RandomNumberGenerator;

/*
  Return an RNG for the given seed, which can either be the global
  RNG or a local one with a user-specified seed. Only use this together
  with "add_rng_options_to_feature()".
*/
extern std::shared_ptr<RandomNumberGenerator> get_rng(int seed);

} // namespace utils

#endif
