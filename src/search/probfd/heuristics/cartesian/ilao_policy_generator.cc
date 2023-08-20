#include "probfd/heuristics/cartesian/ilao_policy_generator.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/trap_aware_dfhs.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/cartesian_abstractions/abstract_state.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

ILAOPolicyGenerator::ILAOPolicyGenerator()
    : ptb(new policy_pickers::
              ArbitraryTiebreaker<int, const ProbabilisticTransition*>(true))
    , picker(new quotients::RepresentativePolicyPicker<
             int,
             const ProbabilisticTransition*>(ptb))
    , report(0.0_vt)
{
    report.disable();
}

unique_ptr<Solution> ILAOPolicyGenerator::find_solution(
    Abstraction& abstraction,
    const AbstractState* state,
    CartesianHeuristic& heuristic,
    utils::CountdownTimer& timer)
{
    // TODO: ideally, this object should not be recreated each time, in
    // particular the storage for the search state. Needs some way to clear
    // the search state.
    engines::trap_aware_dfhs::
        TADepthFirstHeuristicSearch<int, const ProbabilisticTransition*, false>
            hdfs(
                picker,
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

    auto policy = hdfs.compute_policy(
        abstraction,
        heuristic,
        state->get_id(),
        timer.get_remaining_time());

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