#ifndef PROBFD_TASK_UTILS_SAMPLING_H
#define PROBFD_TASK_UTILS_SAMPLING_H

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include <functional>
#include <memory>

namespace downward::utils {
class RandomNumberGenerator;
}


namespace probfd::successor_generator {
class ProbabilisticSuccessorGenerator;
}

namespace probfd::sampling {
/*
  Sample states with random walks.
*/
class RandomWalkSampler {
    const ProbabilisticOperatorsProxy operators;
    downward::AxiomEvaluator& axiom_evaluator;
    const std::unique_ptr<successor_generator::ProbabilisticSuccessorGenerator>
        successor_generator;
    const downward::State initial_state;
    const double average_operator_costs;
    downward::utils::RandomNumberGenerator& rng;

public:
    RandomWalkSampler(
        const ProbabilisticTaskProxy& task_proxy,
        downward::utils::RandomNumberGenerator& rng);
    ~RandomWalkSampler();

    /*
      Perform a single random walk and return the last visited state.

      The walk length is taken from a binomial distribution centered around the
      estimated plan length, which is computed as the ratio of the h value of
      the initial state divided by the average operator costs. Whenever a dead
      end is detected or a state has no successors, restart from the initial
      state. The function 'is_terminal' should return whether a given state is
      a dead end. If omitted, no dead end detection is performed. The 'init_h'
      value should be an estimate of the solution cost.
    */
    downward::State sample_state(
        value_t init_h,
        const std::function<bool(const downward::State&)>& is_dead_end =
            [](const downward::State&) { return false; }) const;
};

} // namespace probfd::sampling

#endif // PROBFD_TASK_UTILS_SAMPLING_H
