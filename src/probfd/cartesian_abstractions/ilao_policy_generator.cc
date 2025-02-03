#include "probfd/cartesian_abstractions/ilao_policy_generator.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/heuristics.h"
#include "probfd/cartesian_abstractions/probabilistic_transition.h"

#include "probfd/algorithms/trap_aware_dfhs.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/policy.h"
#include "probfd/progress_report.h"
#include "probfd/value_type.h"

#include "downward/utils/countdown_timer.h"

#include <optional>

using namespace std;

namespace probfd::cartesian_abstractions {

ILAOPolicyGenerator::ILAOPolicyGenerator(value_t convergence_epsilon)
    : convergence_epsilon_(convergence_epsilon)
    , picker_(new policy_pickers::ArbitraryTiebreaker<
              quotients::QuotientState<int, const ProbabilisticTransition*>,
              quotients::QuotientAction<const ProbabilisticTransition*>>(true))
{
}

unique_ptr<Solution> ILAOPolicyGenerator::find_solution(
    CartesianAbstraction& abstraction,
    const AbstractState* state,
    CartesianHeuristic& heuristic,
    utils::CountdownTimer& timer)
{
    // TODO: ideally, this object should not be recreated each time, in
    // particular the storage for the search state. Needs some way to clear
    // the search state.
    algorithms::trap_aware_dfhs::
        TADepthFirstHeuristicSearch<int, const ProbabilisticTransition*, false>
            talilao(
                convergence_epsilon_,
                picker_,
                false,
                algorithms::trap_aware_dfhs::BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                true,
                true);

    ProgressReport report(0.0_vt);
    report.disable();

    auto policy = talilao.compute_policy(
        abstraction,
        heuristic,
        state->get_id(),
        report,
        timer.get_remaining_time());

    for (int i = 0; i != abstraction.get_num_states(); ++i) {
        if (talilao.was_visited(i)) {
            heuristic.set_h_value(i, talilao.lookup_bounds(i).lower);
        }
    }

    return policy;
}

} // namespace probfd::cartesian_abstractions
