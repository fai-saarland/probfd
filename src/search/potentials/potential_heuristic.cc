#include "potentials/potential_heuristic.h"

#include "potentials/potential_function.h"

#include "plugins/options.h"

using namespace std;

namespace potentials {
PotentialHeuristic::PotentialHeuristic(
    const plugins::Options& opts,
    unique_ptr<PotentialFunction> function)
    : Heuristic(opts)
    , function(std::move(function))
{
}

PotentialHeuristic::~PotentialHeuristic()
{
}

int PotentialHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    return max(0, function->get_value(state));
}
} // namespace potentials
