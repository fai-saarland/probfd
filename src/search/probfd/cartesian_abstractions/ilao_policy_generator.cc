#include "probfd/cartesian_abstractions/ilao_policy_generator.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/abstraction.h"
#include "probfd/cartesian_abstractions/evaluators.h"
#include "probfd/cartesian_abstractions/probabilistic_transition.h"

#include "probfd/algorithms/policy_picker.h"
#include "probfd/algorithms/trap_aware_dfhs.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/policy.h"
#include "probfd/progress_report.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/timer.h"

#include <optional>
#include <set>

using namespace std;

namespace probfd {
namespace cartesian_abstractions {

ILAOPolicyGenerator::ILAOPolicyGenerator()
    : picker(new policy_pickers::ArbitraryTiebreaker<
             quotients::QuotientState<int, const ProbabilisticTransition*>,
             quotients::QuotientAction<const ProbabilisticTransition*>>(true))
{
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
    algorithms::trap_aware_dfhs::
        TADepthFirstHeuristicSearch<int, const ProbabilisticTransition*, false>
            hdfs(
                picker,
                false,
                algorithms::trap_aware_dfhs::BacktrackingUpdateType::SINGLE,
                false,
                false,
                false,
                true,
                false,
                true,
                nullptr);

    ProgressReport report(0.0_vt);
    report.disable();

    auto policy = hdfs.compute_policy(
        abstraction,
        heuristic,
        state->get_id(),
        report,
        timer.get_remaining_time());

    for (int i = 0; i != abstraction.get_num_states(); ++i) {
        if (hdfs.was_visited(i)) {
            heuristic.set_h_value(i, hdfs.lookup_value(i));
        }
    }

    return policy;
}

} // namespace cartesian_abstractions
} // namespace probfd