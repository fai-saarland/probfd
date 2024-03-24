#ifndef PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

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

enum class BacktrackingUpdateType {
    DISABLED,
    ON_DEMAND,
    SINGLE,
    CONVERGENCE,
};

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long forward_updates = 0;
    unsigned long long backtracking_updates = 0;
    unsigned long long convergence_updates = 0;
    unsigned long long backtracking_value_iterations = 0;
    unsigned long long convergence_value_iterations = 0;

    void print(std::ostream& out) const;
};

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t INITIALIZED = 1 << StateInfo::BITS;
    static constexpr uint8_t SOLVED = 2 << StateInfo::BITS;
    static constexpr uint8_t MASK = 3 << StateInfo::BITS;
    static constexpr uint8_t BITS = StateInfo::BITS + 2;

    [[nodiscard]]
    bool is_policy_initialized() const
    {
        return (this->info & MASK) != 0;
    }

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED;
    }

    void set_policy_initialized()
    {
        this->info = (this->info & ~MASK) | INITIALIZED;
    }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }

    void clear() { this->info &= MASK; }
};

enum StateStatus {
    NEW = 0,
    ONSTACK = 1,
    CLOSED = 2,
    UNSOLVED = 3,
    UNDEF = std::numeric_limits<unsigned>::max() >> 1
};

struct LocalStateInfo {
    uint8_t status = StateStatus::NEW;
    unsigned index = StateStatus::UNDEF;
    unsigned lowlink = StateStatus::UNDEF;

    void open(unsigned stack_index)
    {
        status = ONSTACK;
        index = stack_index;
        lowlink = stack_index;
    }
};

struct ExpansionInfo {
    ExpansionInfo(StateID state, const Distribution<StateID>& t)
        : stateid(state)
    {
        for (const StateID s : t.support()) {
            successors.push_back(s);
        }
    }

    const StateID stateid;

    std::vector<StateID> successors;

    bool unsolved_succs = false;
    bool value_changed = false;

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
    : public heuristic_search::HeuristicSearchAlgorithmExt<
          State,
          Action,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using Base = typename HeuristicDepthFirstSearch::HeuristicSearchAlgorithm;

public:
    using StateInfo = typename Base::StateInfo;

private:
    using MDP = typename Base::MDPType;
    using Evaluator = typename Base::EvaluatorType;

    using PolicyPicker = typename Base::PolicyPicker;

    using Statistics = internal::Statistics;
    using ExpansionInfo = internal::ExpansionInfo;
    using LocalStateInfo = internal::LocalStateInfo;

    const bool label_solved_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_inconsistent_;
    const bool greedy_exploration_;
    const bool perform_value_iteration_;
    const bool expand_tip_states_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;

    Statistics statistics_;

public:
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool label_solved,
        bool forward_updates,
        BacktrackingUpdateType backward_updates,
        bool cutoff_inconsistent,
        bool greedy_exploration,
        bool perform_value_iteration,
        bool expand_tip_states);

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

    bool push_successor(
        MDP& mdp,
        Evaluator& heuristic,
        ExpansionInfo& einfo,
        LocalStateInfo& sinfo,
        bool& keep_expanding,
        utils::CountdownTimer& timer);

    uint8_t push(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        StateInfo& sinfo,
        bool& parent_value_changed);

    bool value_iteration(
        MDP& mdp,
        Evaluator& heuristic,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer);

    std::pair<bool, bool> backtracking_value_iteration(
        MDP& mdp,
        Evaluator& heuristic,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer);

    std::pair<bool, bool> vi_step(
        MDP& mdp,
        Evaluator& heuristic,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer,
        unsigned long long& stat_counter);
};

} // namespace probfd::algorithms::heuristic_depth_first_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#include "probfd/algorithms/heuristic_depth_first_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

#endif // PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H