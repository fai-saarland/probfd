#ifndef PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#define PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H

#include "probfd/algorithms/ao_search.h"

namespace probfd {
namespace algorithms {

template <typename>
class OpenList;

/// I do not know this algorithm.
namespace exhaustive_ao {

namespace internal {

template <typename StateInfo>
struct PerStateInformation : public ao_search::PerStateInformation<StateInfo> {
    unsigned unsolved : 31 = 0;
    unsigned alive : 1 = 0;
};

} // namespace internal

/**
 * @brief Exhaustive AO* search algorithm.
 *
 * An exhaustive version of the AO* algorithm that computes an optimal value
 * function for every state reachable from the initial state.
 *
 * \todo Add some implementation notes.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam UseInterval - Whether value intervals are used.
 *
 * @remark The search algorithm does not validate that the state space is
 * acyclic. It is an error to invoke this search algorithm on state spaces which
 * contain cycles.
 */
template <typename State, typename Action, bool UseInterval>
class ExhaustiveAOSearch
    : public ao_search::AOBase<
          State,
          Action,
          UseInterval,
          false,
          internal::PerStateInformation> {
    using Base = typename ExhaustiveAOSearch::AOBase;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;
    using PolicyPicker = typename Base::PolicyPicker;

    using OpenList = OpenList<Action>;

    std::shared_ptr<OpenList> open_list_;

public:
    ExhaustiveAOSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        ProgressReport* report,
        bool interval_comparison,
        std::shared_ptr<OpenList> open_list);

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override;
};

} // namespace exhaustive_ao
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#include "probfd/algorithms/exhaustive_ao_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H

#endif // __EXHAUSTIVE_AO_H__