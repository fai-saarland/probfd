#include "probfd/heuristics/cartesian/abstract_search.h"

#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/ta_topological_value_iteration.h"
#include "probfd/engines/trap_aware_dfhs.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "cegar/abstract_state.h"

#include "utils/countdown_timer.h"
#include "utils/iterators.h"
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
    , ptb(new policy_pickers::ArbitraryTiebreaker<
          const AbstractState*,
          const ProbabilisticTransition*>(true))
    , report(0.0_vt)
{
    report.disable();
}

unique_ptr<Solution> AbstractSearch::find_solution(
    Abstraction& abstraction,
    const AbstractState* state,
    utils::CountdownTimer& timer)
{
    quotients::
        QuotientSystem<const AbstractState*, const ProbabilisticTransition*>
            quotient(&abstraction);

    quotients::DefaultQuotientCostFunction<
        const AbstractState*,
        const ProbabilisticTransition*>
        costs(&quotient, &cost_function);

    quotients::RepresentativePolicyPicker<
        const AbstractState*,
        const ProbabilisticTransition*>
        picker(&quotient, ptb);

    engines::trap_aware_dfhs::TADepthFirstHeuristicSearch<
        const AbstractState*,
        const ProbabilisticTransition*,
        false>
        hdfs(
            &quotient,
            &costs,
            &heuristic,
            &picker,
            nullptr,
            &report,
            false,
            &quotient,
            false,
            engines::trap_aware_dfhs::BacktrackingUpdateType::SINGLE,
            false,
            false,
            false,
            true,
            false,
            true,
            nullptr);

    auto policy = hdfs.compute_policy(state, timer.get_remaining_time());

    for (int i = 0; i != abstraction.get_num_states(); ++i) {
        if (hdfs.was_visited(i)) {
            heuristic.set_h_value(i, hdfs.lookup_value(i));
        }
    }

    return policy;
}

void AbstractSearch::notify_split(int v)
{
    heuristic.on_split(v);
}

CartesianHeuristic& AbstractSearch::get_heuristic()
{
    return heuristic;
}

vector<value_t> compute_distances(
    Abstraction& abstraction,
    CartesianHeuristic& heuristic,
    const vector<value_t>& costs)
{
    vector<value_t> values(abstraction.get_num_states(), INFINITE_VALUE);

    CartesianCostFunction cost_function(abstraction, costs);

    preprocessing::QualitativeReachabilityAnalysis<
        const AbstractState*,
        const ProbabilisticTransition*>
        qr_analysis(&abstraction, &cost_function, true);

    std::vector<StateID> pruned_states;
    qr_analysis.run_analysis(
        &abstraction.get_initial_state(),
        utils::discarding_output_iterator{},
        std::back_inserter(pruned_states),
        utils::discarding_output_iterator{});

    for (StateID pruned_id : pruned_states) {
        heuristic.set_h_value(pruned_id, INFINITE_VALUE);
    }

    engines::ta_topological_vi::TATopologicalValueIteration<
        const AbstractState*,
        const ProbabilisticTransition*>
        tvi(&abstraction, &cost_function, &heuristic);

    tvi.solve(abstraction.get_initial_state().get_id(), values);

    return values;
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd