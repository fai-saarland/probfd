#ifndef PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H
#define PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>
#include <iostream>
#include <type_traits>
#include <vector>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

/// Namespace dedicated to Depth-First Heuristic Search.
namespace probfd::algorithms::heuristic_depth_first_search {

enum class BacktrackingUpdateType { DISABLED, ON_DEMAND, SINGLE };

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long forward_updates = 0;
    unsigned long long backtracking_updates = 0;
    unsigned long long convergence_updates = 0;
    unsigned long long convergence_value_iterations = 0;

    void print(std::ostream& out) const;
};

template <typename Action, bool UseInterval>
struct PerStateInformation
    : public heuristic_search::
          PerStateBaseInformation<Action, true, UseInterval> {
    using Base =
        heuristic_search::PerStateBaseInformation<Action, true, UseInterval>;

public:
    static constexpr uint8_t SOLVED = 1 << Base::BITS;
    static constexpr uint8_t MASK = 1 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 1;

    [[nodiscard]]
    bool is_solved() const
    {
        return (this->info & SOLVED) != 0 || this->is_goal_or_terminal();
    }

    void set_solved() { this->info |= SOLVED; }

    void unset_solved() { this->info &= ~SOLVED; }

    void clear() { this->info &= ~MASK; }
};

struct DFSExplorationState {
    DFSExplorationState(StateID state, unsigned stack_index)
        : state_id(state)
        , lowlink(stack_index)
    {
    }

    const StateID state_id;
    uint32_t lowlink;

    std::vector<StateID> successors;

    bool solved : 1 = true;
    bool value_converged : 1 = true;

    bool next_successor();
    [[nodiscard]]
    StateID get_current_successor() const;
};

} // namespace internal

/**
 * @brief Implementation of the depth-first heuristic search algorithm family
 * \cite steinmetz:etal:icaps-16.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 */
template <typename State, typename Action, bool UseInterval>
class HeuristicDepthFirstSearch
    : public heuristic_search::HeuristicSearchAlgorithm<
          State,
          Action,
          internal::PerStateInformation<Action, UseInterval>> {
    using Base =
        typename HeuristicDepthFirstSearch::HeuristicSearchAlgorithm;

public:
    using StateInfo = typename Base::StateInfo;
    using AlgorithmValueType = Base::AlgorithmValueType;

private:
    using MDP = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;

    using PolicyPicker = typename Base::PolicyPicker;

    using Statistics = internal::Statistics;
    using DFSExplorationState = internal::DFSExplorationState;

    static constexpr uint32_t NEW = std::numeric_limits<uint32_t>::max() - 1;
    static constexpr uint32_t CLOSED = std::numeric_limits<uint32_t>::max();

    // Algorithm parameters
    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool label_solved_;

    // Algorithm state
    std::deque<DFSExplorationState> dfs_stack_;
    std::deque<StateID> tarjan_stack_;
    storage::StateHashMap<uint32_t> stack_index_;

    std::vector<StateID> visited_states_;

    // Re-used buffer
    std::vector<TransitionTail<Action>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;
    SuccessorDistribution successor_dist_;

    Statistics statistics_;

public:
    HeuristicDepthFirstSearch(
        value_t epsilon,
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool label_solved);

protected:
    Interval do_solve(
        MDP& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport& progress,
        downward::utils::Duration max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

private:
    void solve_with_vi_termination(
        MDP& mdp,
        HeuristicType& heuristic,
        StateID stateid,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer);

    void solve_without_vi_termination(
        MDP& mdp,
        HeuristicType& heuristic,
        StateID stateid,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer);

    bool policy_exploration(
        MDP& mdp,
        HeuristicType& heuristic,
        StateID state,
        downward::utils::CountdownTimer& timer);

    bool advance(MDP& mdp, DFSExplorationState& einfo, StateInfo& state_info);

    bool push_successor(
        MDP& mdp,
        DFSExplorationState& einfo,
        StateInfo& sinfo,
        downward::utils::CountdownTimer& timer);

    void push(StateID stateid);

    bool initialize(
        MDP& mdp,
        HeuristicType& heuristic,
        DFSExplorationState& einfo,
        StateInfo& sinfo);

    bool value_iteration(
        MDP& mdp,
        const std::ranges::input_range auto& range,
        downward::utils::CountdownTimer& timer);

    std::pair<bool, bool> vi_step(
        MDP& mdp,
        const std::ranges::input_range auto& range,
        downward::utils::CountdownTimer& timer);
};

} // namespace probfd::algorithms::heuristic_depth_first_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#include "probfd/algorithms/depth_first_heuristic_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

#endif // PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H
