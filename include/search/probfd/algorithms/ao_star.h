#ifndef PROBFD_ALGORITHMS_AO_STAR_H
#define PROBFD_ALGORITHMS_AO_STAR_H

#include "probfd/algorithms/ao_search.h"

#include <vector>

// Forward Declarations
namespace probfd::algorithms {
template <typename>
class SuccessorSampler;
}

/// Namespace dedicated to the AO* algorithm.
namespace probfd::algorithms::ao_search::ao_star {

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
 * @remark The search algorithm does not validate that the stateid space is
 * acyclic. It is an error to invoke this search algorithm on stateid spaces
 * which contain cycles.
 */
template <typename State, typename Action, bool UseInterval>
class AOStar
    : public AOBase<
          State,
          Action,
          PerStateInformation<Action, UseInterval, true>> {
    using Base = typename AOStar::AOBase;

    friend Base;

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
    using PolicyPickerType = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using SuccessorSamplerType = SuccessorSampler<Action>;

    // Algorithm parameters
    const std::shared_ptr<SuccessorSamplerType> outcome_selection_;

public:
    AOStar(
        std::shared_ptr<PolicyPickerType> policy_chooser,
        std::shared_ptr<SuccessorSamplerType> outcome_selection);

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

} // namespace probfd::algorithms::ao_search::ao_star

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H
#include "probfd/algorithms/ao_star_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H

#endif // PROBFD_ALGORITHMS_AO_STAR_H