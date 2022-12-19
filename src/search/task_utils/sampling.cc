#include "task_utils/sampling.h"

#include "task_utils/successor_generator.h"

#include "global_operator.h"
#include "global_state.h"
#include "globals.h"
#include "state_registry.h"

#include "utils/memory.h"
#include "utils/rng.h"

using namespace std;


namespace sampling {

static double get_average_operator_cost() {
    double average_operator_cost = 0;
    for (const auto& op : g_operators) {
        average_operator_cost += op.get_cost();
    }
    average_operator_cost /= g_operators.size();
    return average_operator_cost;
}

static GlobalState sample_state_with_random_walk(
    StateRegistry& state_registry,
    const GlobalState &initial_state,
    const successor_generator::SuccessorGenerator<const GlobalOperator*> &successor_generator,
    int init_h,
    double average_operator_cost,
    utils::RandomNumberGenerator &rng,
    function<bool(GlobalState)> is_dead_end) {
    assert(init_h != numeric_limits<int>::max());
    int n;
    if (init_h == 0) {
        n = 10;
    } else {
        /*
          Convert heuristic value into an approximate number of actions
          (does nothing on unit-cost problems).
          average_operator_cost cannot equal 0, as in this case, all operators
          must have costs of 0 and in this case the if-clause triggers.
        */
        assert(average_operator_cost != 0);
        int solution_steps_estimate = int((init_h / average_operator_cost) + 0.5);
        n = 4 * solution_steps_estimate;
    }
    double p = 0.5;
    /* The expected walk length is np = 2 * estimated number of solution steps.
       (We multiply by 2 because the heuristic is underestimating.) */

    // Calculate length of random walk according to a binomial distribution.
    int length = 0;
    for (int j = 0; j < n; ++j) {
        double random = rng(); // [0..1)
        if (random < p)
            ++length;
    }

    // Sample one state with a random walk of length length.
    GlobalState current_state(initial_state);
    vector<const GlobalOperator*> applicable_operators;
    for (int j = 0; j < length; ++j) {
        applicable_operators.clear();
        successor_generator.generate_applicable_ops(current_state,
                                                    applicable_operators);
        // If there are no applicable operators, do not walk further.
        if (applicable_operators.empty()) {
            break;
        } else {
            const GlobalOperator* random_op_id = *rng.choose(applicable_operators);
            current_state = state_registry.get_successor_state(current_state, *random_op_id);
            /* If current state is a dead end, then restart the random walk
               with the initial state. */
            if (is_dead_end(current_state))
                current_state = GlobalState(initial_state);
        }
    }
    // The last state of the random walk is used as a sample.
    return current_state;
}


RandomWalkSampler::RandomWalkSampler(
    StateRegistry& state_registry,
    utils::RandomNumberGenerator &rng)
    : successor_generator(successor_generator::g_successor_generator),
      average_operator_costs(get_average_operator_cost()),
      rng(rng),
      state_registry(state_registry) {
}

RandomWalkSampler::~RandomWalkSampler() {
}

GlobalState RandomWalkSampler::sample_state(
    int init_h, const DeadEndDetector &is_dead_end) const {
    return sample_state_with_random_walk(
        state_registry,
        state_registry.get_initial_state(),
        *successor_generator,
        init_h,
        average_operator_costs,
        rng,
        is_dead_end);
}
}
