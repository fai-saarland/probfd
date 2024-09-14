#ifndef PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H
#define PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>
#include <iostream>
#include <type_traits>
#include <vector>

// Forward Declarations
namespace utils {
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
    using Base = PerStateInformation::PerStateBaseInformation;

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

struct LocalStateInfo {
    enum { NEW = 0, ONSTACK = 1, CLOSED = 2 };

    uint8_t status = NEW;
    unsigned index = std::numeric_limits<unsigned>::max();
    unsigned lowlink = std::numeric_limits<unsigned>::max();

    void open(unsigned stack_index)
    {
        index = stack_index;
        lowlink = stack_index;
    }
};

struct ExpansionInfo {
    explicit ExpansionInfo(StateID state)
        : stateid(state)
    {
    }

    const StateID stateid;

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
 * @tparam Fret - Whether the algorithm is wrapped with the FRET framework.
 */
template <typename State, typename Action, bool UseInterval>
class HeuristicDepthFirstSearch
    : public heuristic_search::FRETHeuristicSearchAlgorithm<
          State,
          Action,
          internal::PerStateInformation<Action, UseInterval>> {
    using Base =
        typename HeuristicDepthFirstSearch::FRETHeuristicSearchAlgorithm;

public:
    using StateInfo = typename Base::StateInfo;
    using AlgorithmValueType = Base::AlgorithmValueType;

private:
    using MDP = typename Base::MDPType;
    using Evaluator = typename Base::EvaluatorType;

    using PolicyPicker = typename Base::PolicyPicker;

    using Statistics = internal::Statistics;
    using ExpansionInfo = internal::ExpansionInfo;
    using LocalStateInfo = internal::LocalStateInfo;

    // Algorithm parameters
    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_on_cutoff_;
    const bool value_iteration_;
    const bool label_solved_;

    // Algorithm state
    storage::StateHashMap<LocalStateInfo> local_state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;

    Statistics statistics_;

    // Re-used buffer
    std::vector<Transition<Action>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;

public:
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool terminate_exploration_on_cutoff,
        bool value_iteration,
        bool label_solved);

    void reset_search_state() override;

protected:
    Interval do_solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport& progress,
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

private:
    void solve_with_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    void solve_without_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    template <bool GetVisited>
    bool policy_exploration(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        utils::CountdownTimer& timer);

    bool advance(MDP& mdp, ExpansionInfo& einfo, StateInfo& state_info);

    bool push_successor(
        MDP& mdp,
        ExpansionInfo& einfo,
        StateInfo& sinfo,
        LocalStateInfo& lsinfo,
        utils::CountdownTimer& timer);

    void push(StateID stateid);

    bool initialize(
        MDP& mdp,
        Evaluator& heuristic,
        ExpansionInfo& einfo,
        StateInfo& sinfo);

    bool value_iteration(
        MDP& mdp,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer);

    std::pair<bool, bool> vi_step(
        MDP& mdp,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer,
        unsigned long long& stat_counter);
};

} // namespace probfd::algorithms::heuristic_depth_first_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#include "probfd/algorithms/depth_first_heuristic_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

#endif // PROBFD_ALGORITHMS_DEPTH_FIRST_HEURISTIC_SEARCH_H