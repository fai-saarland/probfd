#include "downward/potentials/potential_max_heuristic.h"

#include "downward/potentials/potential_function.h"

#include "downward/plugins/options.h"

using namespace std;

namespace potentials {
PotentialMaxHeuristic::PotentialMaxHeuristic(
    const plugins::Options& opts,
    vector<unique_ptr<PotentialFunction>>&& functions)
    : Heuristic(opts)
    , functions(std::move(functions))
{
}

int PotentialMaxHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int value = 0;
    for (auto& function : functions) {
        value = max(value, function->get_value(state));
    }
    return value;
}
} // namespace potentials
