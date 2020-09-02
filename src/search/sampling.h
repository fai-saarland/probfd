#ifndef TASK_UTILS_SAMPLING_H
#define TASK_UTILS_SAMPLING_H

#include <functional>
#include <memory>

class GlobalState;
class GlobalOperator;
class StateRegistry;

namespace successor_generator {
template<typename T> class SuccessorGenerator;
}

namespace utils {
class RandomNumberGenerator;
}

using DeadEndDetector = std::function<bool (GlobalState)>;

namespace sampling {
/*
  Sample states with random walks.
*/
class RandomWalkSampler {
    const std::shared_ptr<successor_generator::SuccessorGenerator<const GlobalOperator*> > successor_generator;
    const double average_operator_costs;
    utils::RandomNumberGenerator &rng;
    StateRegistry& state_registry;

public:
    RandomWalkSampler(
        StateRegistry& state_registry,
        utils::RandomNumberGenerator &rng);
    ~RandomWalkSampler();

    /*
      Perform a single random walk and return the last visited state.

      The walk length is taken from a binomial distribution centered around the
      estimated plan length, which is computed as the ratio of the h value of
      the initial state divided by the average operator costs. Whenever a dead
      end is detected or a state has no successors, restart from the initial
      state. The function 'is_dead_end' should return whether a given state is
      a dead end. If omitted, no dead end detection is performed. The 'init_h'
      value should be an estimate of the solution cost.
    */
    GlobalState sample_state(
        int init_h,
        const DeadEndDetector &is_dead_end = [](const GlobalState &) {return false;}) const;
};
}

#endif
