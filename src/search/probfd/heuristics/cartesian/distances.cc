#include "probfd/heuristics/cartesian/distances.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/evaluators.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

vector<value_t>
compute_distances(Abstraction& abstraction, CartesianHeuristic& heuristic)
{
    vector<value_t> values(abstraction.get_num_states(), INFINITE_VALUE);

    preprocessing::
        QualitativeReachabilityAnalysis<int, const ProbabilisticTransition*>
            qr_analysis(true);

    std::vector<StateID> pruned_states;
    qr_analysis.run_analysis(
        abstraction,
        nullptr,
        abstraction.get_initial_state().get_id(),
        iterators::discarding_output_iterator{},
        std::back_inserter(pruned_states),
        iterators::discarding_output_iterator{});

    for (StateID pruned_id : pruned_states) {
        heuristic.set_h_value(pruned_id, INFINITE_VALUE);
    }

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

} // namespace cartesian
} // namespace heuristics
} // namespace probfd