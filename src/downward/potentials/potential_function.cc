#include "downward/potentials/potential_function.h"

#include "downward/task_proxy.h"

#include "downward/utils/collections.h"

#include <cmath>

using namespace std;

namespace downward::potentials {
PotentialFunction::PotentialFunction(
    const vector<vector<double>>& fact_potentials)
    : fact_potentials(fact_potentials)
{
}

int PotentialFunction::get_value(const State& state) const
{
    double heuristic_value = 0.0;
    for (const auto [var_id, value] : state | as_fact_pair_set) {
        assert(utils::in_bounds(var_id, fact_potentials));
        assert(utils::in_bounds(value, fact_potentials[var_id]));
        heuristic_value += fact_potentials[var_id][value];
    }
    const double epsilon = 0.01;
    return static_cast<int>(ceil(heuristic_value - epsilon));
}
} // namespace potentials
