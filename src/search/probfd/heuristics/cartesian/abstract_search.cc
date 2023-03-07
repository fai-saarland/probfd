#include "probfd/heuristics/cartesian/abstract_search.h"

#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/heuristic_depth_first_search.h"
#include "probfd/engines/topological_value_iteration.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "cegar/abstract_state.h"

#include "utils/memory.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

AbstractSearch::AbstractSearch(
    Abstraction& abstraction,
    std::vector<value_t> operator_costs)
    : cost_function(abstraction, std::move(operator_costs))
{
}

unique_ptr<Solution> AbstractSearch::find_solution(
    Abstraction& abstraction,
    const AbstractState* state)
{
    ProgressReport report(0.0_vt);

    engine_interfaces::
        StateSpace<const AbstractState*, const ProbabilisticTransition*>
            state_space(abstraction);
    policy_pickers::ArbitraryTiebreaker<
        const AbstractState*,
        const ProbabilisticTransition*>
        ptb(true);

    engines::heuristic_depth_first_search::HeuristicDepthFirstSearch<
        const AbstractState*,
        const ProbabilisticTransition*,
        false,
        false>
        hdfs(
            &state_space,
            &cost_function,
            &heuristic,
            &ptb,
            nullptr,
            &report,
            false,
            false,
            false,
            engines::heuristic_depth_first_search::BacktrackingUpdateType::
                SINGLE,
            false,
            false,
            true,
            false);

    auto policy = hdfs.compute_policy(state);
    return policy;
}

vector<value_t>
compute_distances(Abstraction& abstraction, const vector<value_t>& costs)
{
    vector<value_t> new_values(abstraction.get_num_states(), INFINITE_VALUE);

    engine_interfaces::
        StateSpace<const AbstractState*, const ProbabilisticTransition*>
            state_space(abstraction);

    CartesianCostFunction cost_function(abstraction, costs);
    CartesianHeuristic heuristic;

    engines::topological_vi::TopologicalValueIteration<
        const AbstractState*,
        const ProbabilisticTransition*>
        tvi(&state_space, &cost_function, &heuristic, true);

    tvi.solve(abstraction.get_initial_state().get_id(), new_values);

    return new_values;
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd