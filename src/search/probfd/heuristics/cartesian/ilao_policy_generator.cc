#include "probfd/heuristics/cartesian/ilao_policy_generator.h"

#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/trap_aware_dfhs.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/cartesian_abstractions/abstract_state.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

ILAOPolicyGenerator::ILAOPolicyGenerator()
    : ptb(new policy_pickers::ArbitraryTiebreaker<
          const AbstractState*,
          const ProbabilisticTransition*>(true))
    , report(0.0_vt)
{
    report.disable();
}

unique_ptr<Solution> ILAOPolicyGenerator::find_solution(
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    const AbstractState* state,
    CartesianHeuristic& heuristic,
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

} // namespace cartesian
} // namespace heuristics
} // namespace probfd