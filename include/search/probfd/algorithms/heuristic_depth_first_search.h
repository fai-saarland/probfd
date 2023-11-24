#ifndef PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

#include "probfd/algorithms/heuristic_search_base.h"

#include <deque>
#include <iostream>
#include <type_traits>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace algorithms {

/// Namespace dedicated to Depth-First Heuristic Search.
namespace heuristic_depth_first_search {

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

    bool is_policy_initialized() const { return (this->info & MASK) != 0; }
    bool is_solved() const { return this->info & SOLVED; }
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
    CLOSED_DEAD = 3,
    UNSOLVED = 4,
    UNDEF = ((unsigned)-1) >> 1
};

struct LocalStateInfo {
    uint8_t status = StateStatus::NEW;
    unsigned index = StateStatus::UNDEF;
    unsigned lowlink = StateStatus::UNDEF;

    void open(const unsigned& index)
    {
        status = ONSTACK;
        this->index = index;
        this->lowlink = index;
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

    bool dead = true;
    bool unsolved_succs = false;
    bool value_changed = false;

    bool next_successor();
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
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using PolicyPicker = typename Base::PolicyPicker;

    using Statistics = internal::Statistics;
    using ExpansionInfo = internal::ExpansionInfo;
    using LocalStateInfo = internal::LocalStateInfo;

    const bool LabelSolved;
    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;

    Statistics statistics_;

public:
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        bool LabelSolved,
        bool ForwardUpdates,
        BacktrackingUpdateType BackwardUpdates,
        bool CutoffInconsistent,
        bool GreedyExploration,
        bool PerformValueIteration,
        bool ExpandTipStates);

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
        bool& parent_value_changed,
        bool& parent_unsolved_successors);

    std::pair<bool, bool> value_iteration(
        MDP& mdp,
        Evaluator& heuristic,
        const std::ranges::input_range auto& range,
        bool until_convergence,
        utils::CountdownTimer& timer);
};

} // namespace heuristic_depth_first_search
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H
#include "probfd/algorithms/heuristic_depth_first_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_DEPTH_FIRST_SEARCH_H

#endif // __HEURISTIC_DEPTH_FIRST_SEARCH_H__