#ifndef PROBFD_ENGINES_AO_STAR_H
#define PROBFD_ENGINES_AO_STAR_H

#include "probfd/engines/ao_search.h"

#include <vector>

namespace probfd {

namespace engine_interfaces {
template <typename>
class SuccessorSampler;
}

namespace engines {

/// Namespace dedicated to the AO* algorithm.
namespace ao_search {
namespace ao_star {

/**
 * @brief Implementation of the AO* algorithm.
 *
 * The AO* algorithm is an MDP heuristic search algorithm applicable only to
 * acyclic MDPs. The algorithm maintains a greedy policy and iteratively
 * constructs a subgraph of the MDP. In each iteration, a non-terminal leaf
 * stateid of the subgraph that is reachable by the greedy policy (a tip
 * stateid) is sampled and expanded. Afterwards, Bellman updates are performed
 * on the backward-reachable subgraph ending in the sampled stateid in reverse
 * topological order, while also updating the greedy policy. The algorithm
 * terminates once no tip states are left, i.e. the greedy policy is fully
 * explored.
 *
 * @tparam State - The stateid type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 * @remark The search engine does not validate that the stateid space is
 * acyclic. It is an error to invoke this search engine on stateid spaces which
 * contain cycles.
 */
template <typename State, typename Action, bool UseInterval>
class AOStar
    : public AOBase<State, Action, UseInterval, true, PerStateInformation> {
    using Base = typename AOStar::AOBase;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;
    using PolicyPicker = typename Base::PolicyPicker;
    using NewStateObserver = typename Base::NewStateObserver;

    using SuccessorSampler = engine_interfaces::SuccessorSampler<Action>;

    std::shared_ptr<SuccessorSampler> outcome_selection_;

    Distribution<StateID> selected_transition_;

public:
    AOStar(
        std::shared_ptr<PolicyPicker> policy_chooser,
        std::shared_ptr<NewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        std::shared_ptr<SuccessorSampler> outcome_selection);

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override;
};

} // namespace ao_star
} // namespace ao_search
} // namespace engines
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ENGINES_AO_STAR_H
#include "probfd/engines/ao_star_impl.h"
#undef GUARD_INCLUDE_PROBFD_ENGINES_AO_STAR_H

#endif // __AO_STAR_H__