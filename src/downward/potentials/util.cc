#include "downward/potentials/util.h"

#include "downward/potentials/fact_potential_function.h"
#include "downward/potentials/potential_optimizer.h"

#include "downward/task_utils/sampling.h"

#include "downward/heuristic.h"
#include "downward/initial_state_values.h"

using namespace std;

namespace downward::potentials {

vector<State> sample_without_dead_end_detection(
    PotentialOptimizer& optimizer,
    int num_samples,
    utils::RandomNumberGenerator& rng)
{
    const SharedAbstractTask task = optimizer.get_task();

    const auto& variables = get_variables(task);
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& init_vals = get_init(task);
    const auto& cost_function = get_cost_function(task);

    const State initial_state = init_vals.get_initial_state();
    const int init_h =
        optimizer.optimize_for_state(initial_state).get_value(initial_state);
    AxiomEvaluator& evaluator = g_axiom_evaluators[variables, axioms];
    const sampling::RandomWalkSampler
        sampler(evaluator, variables, operators, cost_function, rng);

    vector<State> samples;
    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        samples.push_back(sampler.sample_state(init_h, initial_state));
    }
    return samples;
}

} // namespace downward::potentials
