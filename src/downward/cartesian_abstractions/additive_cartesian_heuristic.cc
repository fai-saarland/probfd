#include "downward/cartesian_abstractions/additive_cartesian_heuristic.h"

#include "downward/cartesian_abstractions/cartesian_heuristic_function.h"
#include "downward/cartesian_abstractions/cost_saturation.h"
#include "downward/cartesian_abstractions/types.h"
#include "downward/cartesian_abstractions/utils.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <cassert>

using namespace std;

namespace cartesian_abstractions {
static vector<CartesianHeuristicFunction> generate_heuristic_functions(
    const vector<shared_ptr<SubtaskGenerator>>& subtask_generators,
    int max_states,
    int max_transitions,
    double max_time,
    PickSplit pick,
    bool use_general_costs,
    int random_seed,
    const shared_ptr<AbstractTask>& transform,
    utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Initializing additive Cartesian heuristic..." << endl;
    }
    shared_ptr<utils::RandomNumberGenerator> rng = utils::get_rng(random_seed);
    CostSaturation cost_saturation(
        subtask_generators,
        max_states,
        max_transitions,
        max_time,
        pick,
        use_general_costs,
        *rng,
        log);
    return cost_saturation.generate_heuristic_functions(transform);
}

AdditiveCartesianHeuristic::AdditiveCartesianHeuristic(
    const vector<shared_ptr<SubtaskGenerator>>& subtasks,
    int max_states,
    int max_transitions,
    double max_time,
    PickSplit pick,
    bool use_general_costs,
    int random_seed,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , heuristic_functions(generate_heuristic_functions(
          subtasks,
          max_states,
          max_transitions,
          max_time,
          pick,
          use_general_costs,
          random_seed,
          transform,
          log))
{
}

int AdditiveCartesianHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int sum_h = 0;
    for (const CartesianHeuristicFunction& function : heuristic_functions) {
        int value = function.get_value(state);
        assert(value >= 0);
        if (value == INF) return DEAD_END;
        sum_h += value;
    }
    assert(sum_h >= 0);
    return sum_h;
}

} // namespace cartesian_abstractions
