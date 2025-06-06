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
    for (FactProxy fact : state) {
        int var_id = fact.get_variable().get_id();
        int value = fact.get_value();
        assert(utils::in_bounds(var_id, fact_potentials));
        assert(utils::in_bounds(value, fact_potentials[var_id]));
        heuristic_value += fact_potentials[var_id][value];
    }
    const double epsilon = 0.01;
    return static_cast<int>(ceil(heuristic_value - epsilon));
}
} // namespace potentials
