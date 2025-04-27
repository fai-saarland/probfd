#ifndef TASK_UTILS_SAMPLING_H
#define TASK_UTILS_SAMPLING_H

#include "downward/abstract_task.h"
#include "downward/classical_operator_space.h"
#include "downward/operator_cost_function_fwd.h"
#include "downward/state.h"

#include <functional>
#include <memory>

namespace downward {
class State;
}

namespace downward::successor_generator {
class SuccessorGenerator;
}

namespace downward::utils {
class RandomNumberGenerator;
}

using DeadEndDetector = std::function<bool(downward::State)>;

namespace downward::sampling {
/*
  Sample states with random walks.
*/
class RandomWalkSampler {
    AxiomEvaluator& axiom_evaluator;
    const ClassicalOperatorSpace& operators;
    const std::unique_ptr<successor_generator::SuccessorGenerator>
        successor_generator;
    const double average_operator_costs;
    utils::RandomNumberGenerator& rng;

public:
    RandomWalkSampler(
        AxiomEvaluator& axiom_evaluator,
        const VariableSpace& variables,
        const ClassicalOperatorSpace& operators,
        const OperatorIntCostFunction& cost_function,
        utils::RandomNumberGenerator& rng);

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
    State sample_state(
        int init_h,
        const State& initial_state,
        const DeadEndDetector& is_dead_end = [](const State&) {
            return false;
        }) const;
};
} // namespace downward::sampling

#endif
