#ifndef PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#define PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H

#include "probfd/algorithms/ao_search.h"

// Forward Declarations
namespace probfd::algorithms {
template <typename>
class OpenList;
}

/// I do not know this algorithm.
namespace probfd::algorithms::exhaustive_ao {

namespace internal {

template <typename Action, bool UseInterval>
struct PerStateInformation
    : public ao_search::PerStateInformation<Action, UseInterval, false> {
    unsigned unsolved = 0;
};

} // namespace internal

/**
 * @brief Exhaustive AO* search algorithm.
 *
 * An exhaustive version of the AO* algorithm that computes an optimal value
 * function for every state reachable from the initial state.
 *
 * @todo Add some implementation notes.
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
          internal::PerStateInformation<Action, UseInterval>> {
    using Base = typename ExhaustiveAOSearch::AOBase;

    friend Base;

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
    using PolicyPickerType = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using OpenListType = OpenList<Action>;

    // Algorithm parameters
    const std::shared_ptr<OpenListType> open_list_;

public:
    ExhaustiveAOSearch(
        std::shared_ptr<PolicyPickerType> policy_chooser,
        std::shared_ptr<OpenListType> open_list);

protected:
    Interval do_solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> initial_state,
        ProgressReport& progress,
        double max_time) override;

private:
    bool update_value_check_solved(
        MDPType& mdp,
        EvaluatorType& heuristic,
        StateID state,
        StateInfo& info);
};

} // namespace probfd::algorithms::exhaustive_ao

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H
#include "probfd/algorithms/exhaustive_ao_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H

#endif // PROBFD_ALGORITHMS_EXHAUSTIVE_AO_H