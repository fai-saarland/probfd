#ifndef PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#define PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H

#include "probfd/engines/heuristic_search_base.h"

#include <deque>
#include <iostream>
#include <type_traits>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace engines {

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

struct PerStateInformationBase {
    static constexpr uint8_t BITS = 0;
    uint8_t info = 0;
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
};

using StandalonePerStateInformation =
    PerStateInformation<PerStateInformationBase>;

// Store HDFS-specific state information seperately when FRET is enabled to
// make resetting the solver state easier.
template <typename State, typename Action, bool UseInterval, bool Fret>
using HDFSBase = std::conditional_t<
    Fret,
    heuristic_search::HeuristicSearchEngine<
        State,
        Action,
        UseInterval,
        true,
        heuristic_search::NoAdditionalStateData>,
    heuristic_search::HeuristicSearchEngine<
        State,
        Action,
        UseInterval,
        true,
        internal::PerStateInformation>>;

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
template <typename State, typename Action, bool UseInterval, bool Fret>
class HeuristicDepthFirstSearch
    : public internal::HDFSBase<State, Action, UseInterval, Fret> {
    using Base = typename HeuristicDepthFirstSearch::HeuristicSearchEngine;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using PolicyPicker = typename Base::PolicyPicker;
    using NewStateObserver = typename Base::NewStateObserver;

    using StateInfo = typename Base::StateInfo;

    using Statistics = internal::Statistics;

    using AdditionalStateInfo = std::
        conditional_t<Fret, internal::StandalonePerStateInformation, StateInfo>;

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
        bool leaf = true;
    };

    const bool LabelSolved;
    const bool ForwardUpdates;
    const BacktrackingUpdateType BackwardUpdates;
    const bool CutoffInconsistent;
    const bool GreedyExploration;
    const bool PerformValueIteration;
    const bool ExpandTipStates;

    storage::PerStateStorage<AdditionalStateInfo> state_flags_;

    storage::StateHashMap<LocalStateInfo> state_infos_;
    std::vector<StateID> visited_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StateID> stack_;

    Statistics statistics_;

public:
    HeuristicDepthFirstSearch(
        std::shared_ptr<PolicyPicker> policy_chooser,
        std::shared_ptr<NewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
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
        double max_time) override;

    void print_additional_statistics(std::ostream& out) const override;

private:
    AdditionalStateInfo& get_pers_info(StateID state_id);

    void solve_with_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        utils::CountdownTimer& timer);

    void solve_without_vi_termination(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        utils::CountdownTimer& timer);

    template <bool GetVisited>
    bool policy_exploration(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state,
        utils::CountdownTimer& timer);

    uint8_t push(
        MDP& mdp,
        Evaluator& heuristic,
        StateID stateid,
        AdditionalStateInfo& sinfo,
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
} // namespace engines
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H
#include "probfd/engines/heuristic_depth_first_search_impl.h"
#undef GUARD_INCLUDE_PROBFD_ENGINES_HEURISTIC_DEPTH_FIRST_SEARCH_H

#endif // __HEURISTIC_DEPTH_FIRST_SEARCH_H__