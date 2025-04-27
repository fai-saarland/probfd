#include "probfd/task_utils/sampling.h"

#include "probfd/task_utils/probabilistic_successor_generator.h"
#include "probfd/task_utils/task_properties.h"

#include "probfd/probabilistic_task.h"

#include "downward/axioms.h"
#include "downward/per_task_information.h"

#include "downward/utils/memory.h"
#include "downward/utils/rng.h"

using namespace std;

using namespace downward;

using namespace probfd::successor_generator;

namespace probfd::sampling {

static State sample_state_with_random_walk(
    AxiomEvaluator& axiom_evaluator,
    const ProbabilisticOperatorSpace& operators,
    const State& initial_state,
    const ProbabilisticSuccessorGenerator& successor_generator,
    value_t init_h,
    value_t average_operator_cost,
    utils::RandomNumberGenerator& rng,
    const function<bool(State)>& is_dead_end)
{
    assert(init_h != INFINITE_VALUE);

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
        int solution_steps_estimate =
            static_cast<int>(std::round(init_h / average_operator_cost));
        n = 4 * solution_steps_estimate;
    }

    double p = 0.5;
    /* The expected walk length is np = 2 * estimated number of solution steps.
       (We multiply by 2 because the heuristic is underestimating.) */

    // Calculate length of random walk according to a binomial distribution.
    int length = 0;
    for (int j = 0; j < n; ++j) {
        double random = rng.random(); // [0..1)
        if (random < p) ++length;
    }

    // Sample one state with a random walk of length length.
    State current_state(initial_state);
    vector<OperatorID> applicable_operators;
    for (int j = 0; j < length; ++j) {
        applicable_operators.clear();
        successor_generator.generate_applicable_ops(
            current_state,
            applicable_operators);
        // If there are no applicable operators, do not walk further.
        if (applicable_operators.empty()) { break; }

        OperatorID random_op_id = *rng.choose(applicable_operators);
        ProbabilisticOperatorProxy random_op = operators[random_op_id];
        double r = rng.random();
        for (ProbabilisticOutcomeProxy outcome : random_op.get_outcomes()) {
            r -= outcome.get_probability();
            if (r < 0.0) {
                assert(
                    task_properties::is_applicable(random_op, current_state));
                current_state = current_state.get_unregistered_successor(
                    axiom_evaluator,
                    outcome);
                /* If current state is a dead end, then restart the random walk
                   with the initial state. */
                if (is_dead_end(current_state)) {
                    current_state = State(initial_state);
                }
                goto continue_outer;
            }
        }
        abort();

    continue_outer:;
    }
    // The last state of the random walk is used as a sample.
    return current_state;
}

RandomWalkSampler::RandomWalkSampler(
    const VariableSpace& variables,
    const ProbabilisticOperatorSpace& operators,
    const OperatorCostFunction<value_t>& cost_function,
    AxiomEvaluator& evaluator,
    utils::RandomNumberGenerator& rng)
    : operators(operators)
    , axiom_evaluator(evaluator)
    , successor_generator(
          std::make_unique<
              successor_generator::ProbabilisticSuccessorGenerator>(variables, operators))
    , average_operator_costs(
          task_properties::get_average_operator_cost(operators, cost_function))
    , rng(rng)
{
}

RandomWalkSampler::~RandomWalkSampler() = default;

State RandomWalkSampler::sample_state(
    value_t init_h,
    const State& initial_state,
    const std::function<bool(const State&)>& is_dead_end) const
{
    return sample_state_with_random_walk(
        axiom_evaluator,
        operators,
        initial_state,
        *successor_generator,
        init_h,
        average_operator_costs,
        rng,
        is_dead_end);
}
} // namespace probfd::sampling
