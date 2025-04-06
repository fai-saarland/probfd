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
 * state_id of the subgraph that is reachable by the greedy policy (a tip
 * state_id) is sampled and expanded. Afterwards, Bellman updates are performed
 * on the backward-reachable subgraph ending in the sampled state_id in reverse
 * topological order, while also updating the greedy policy. The algorithm
 * terminates once no tip states are left, i.e. the greedy policy is fully
 * explored.
 *
 * @tparam State - The state_id type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 * @remark The search algorithm does not validate that the state_id space is
 * acyclic. It is an error to invoke this search algorithm on state_id spaces
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

    using AlgorithmValueType = typename Base::AlgorithmValueType;

    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;
    using PolicyPickerType = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using SuccessorSamplerType = SuccessorSampler<Action>;

    // Algorithm parameters
    const std::shared_ptr<SuccessorSamplerType> outcome_selection_;

    // Re-used buffers
    std::vector<TransitionTail<Action>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;
    SuccessorDistribution successor_dist_;

public:
    AOStar(
        value_t epsilon,
        std::shared_ptr<PolicyPickerType> policy_chooser,
        std::shared_ptr<SuccessorSamplerType> outcome_selection);

protected:
    Interval do_solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> initial_state,
        ProgressReport& progress,
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

private:
    bool update_value_check_solved(
        MDPType& mdp,
        ParamType<State> state,
        std::vector<TransitionTail<Action>> transitions,
        StateInfo& info);
};

} // namespace probfd::algorithms::ao_search::ao_star

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H
#include "probfd/algorithms/ao_star_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_AO_STAR_H

#endif // PROBFD_ALGORITHMS_AO_STAR_H