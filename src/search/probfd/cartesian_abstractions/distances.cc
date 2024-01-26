#include "probfd/cartesian_abstractions/distances.h"

#include "probfd/cartesian_abstractions/abstraction.h"
#include "probfd/cartesian_abstractions/evaluators.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

using namespace std;

namespace probfd::cartesian_abstractions {

struct ProbabilisticTransition;

vector<value_t>
compute_distances(Abstraction& abstraction, CartesianHeuristic& heuristic)
{
    vector<value_t> values(abstraction.get_num_states(), INFINITE_VALUE);

    algorithms::ta_topological_vi::
        TATopologicalValueIteration<int, const ProbabilisticTransition*>
            tvi;

    tvi.solve(
        abstraction,
        heuristic,
        abstraction.get_initial_state().get_id(),
        values);

    return values;
}

} // namespace probfd::cartesian_abstractions
