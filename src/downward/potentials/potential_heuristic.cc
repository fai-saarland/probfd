#include "downward/potentials/potential_heuristic.h"

#include "downward/potentials/potential_function.h"

#include "downward/plugins/options.h"

using namespace std;

namespace potentials {
PotentialHeuristic::PotentialHeuristic(
    unique_ptr<PotentialFunction> function,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , function(std::move(function))
{
}

int PotentialHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    return max(0, function->get_value(state));
}
} // namespace potentials
