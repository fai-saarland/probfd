#ifndef PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#define PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#include "probfd/algorithms/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/distribution.h"

#include "downward/utils/timer.h"

#include <type_traits>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace algorithms {

template <typename>
class OpenList;

/// Namespace dedicated to the depth-first heuristic search (DFHS) family with
/// native trap handling support.
namespace trap_aware_dfhs {

enum class BacktrackingUpdateType {
    DISABLED,
    ON_DEMAND,
    SINGLE,
    CONVERGENCE,
};

namespace internal {

struct Statistics {
    utils::Timer trap_timer = utils::Timer(true);
    unsigned long long iterations = 0;
    unsigned long long traps = 0;
    unsigned long long reexpansions = 0;
    unsigned long long fw_updates = 0;
    unsigned long long bw_updates = 0;

    void print(std::ostream& out) const;
    void register_report(ProgressReport& report) const;
};

template <typename BaseInfo>
struct PerStateInformation : public BaseInfo {
    static constexpr uint8_t SOLVED = (1 << BaseInfo::BITS);
    static constexpr uint8_t BITS = BaseInfo::BITS + 1;
    static constexpr uint8_t MASK = (1 << BaseInfo::BITS);

    bool is_solved() const { return this->info & SOLVED; }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
};

} // namespace internal

template <typename State, typename Action, bool UseInterval>
class TADFHSImpl
    : public heuristic_search::HeuristicSearchBaseExt<
          quotients::QuotientState<State, Action>,
          quotients::QuotientAction<Action>,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using Base = typename TADFHSImpl::HeuristicSearchBase;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QEvaluator = typename Base::Evaluator;
    using QuotientPolicyPicker = typename Base::PolicyPicker;
    using UpdateResult = typename Base ::UpdateResult;
    using StateInfo = typename Base::StateInfo;

    using QuotientOpenList = OpenList<QAction>;

    struct Flags {
        /// was the exploration cut off?
        bool complete = true;
        /// were all reached outside-SCC states marked solved?
        bool all_solved = true;
        /// were all reached outside-SCC states marked dead end?
        bool dead = true;
        /// are there any outside-SCC states reachable, and do all transitions
        /// within the SCC generate 0-cost?
        bool is_trap = true;

        void clear()
        {
            complete = true;
            all_solved = true;
            dead = true;
            is_trap = true;
        }

        void update(const StateInfo& info)
        {
            dead = dead && info.is_dead_end();
            is_trap = false;
        }

        void update(const Flags& flags)
        {
            complete = complete && flags.complete;
            all_solved = all_solved && flags.all_solved;
            dead = dead && flags.dead;
            is_trap = is_trap && flags.is_trap;
        }
    };

    struct ExplorationInformation {
        StateID state;
        int lowlink;
        std::vector<StateID> successors;
        Flags flags;

        explicit ExplorationInformation(StateID state, int stack_index)
            : state(state)
            , lowlink(stack_index)
        {
        }
    };

    struct StackInfo {
        StateID state_id;
        std::vector<QAction> aops;

        StackInfo(StateID state_id, QAction action)
            : state_id(state_id)
            , aops({action})
        {
        }

        template <size_t i>
        friend auto& get(StackInfo& info)
        {
            if constexpr (i == 0) return info.state_id;
            if constexpr (i == 1) return info.aops;
        }

        template <size_t i>
        friend const auto& get(const StackInfo& info)
        {
            if constexpr (i == 0) return info.state_id;
            if constexpr (i == 1) return info.aops;
        }
    };

    static constexpr int STATE_UNSEEN = -1;
    static constexpr int STATE_CLOSED = -2;

    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool expand_tip_states_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool mark_solved_;
    const bool reexpand_removed_traps_;

    std::shared_ptr<QuotientOpenList> open_list_;

    bool terminated_ = false;

    std::deque<ExplorationInformation> queue_;
    std::vector<StackInfo> stack_;
    std::vector<StateID> visited_states_;
    storage::StateHashMap<int> stack_index_;

    Distribution<StateID> transition_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADFHSImpl(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool interval_comparison,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        std::shared_ptr<QuotientOpenList> open_list);

    Interval solve_quotient(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        param_type<QState> qstate,
        ProgressReport& progress,
        double max_time);

    void print_statistics(std::ostream& out) const;

private:
    void dfhs_vi_driver(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        const StateID state,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    void dfhs_label_driver(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        const StateID state,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    void enqueue(
        QuotientSystem& quotient,
        StateID state,
        QAction action,
        const Distribution<StateID>& successor_dist);

    bool push_state(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        StateInfo& state_info,
        Flags& flags);

    bool push_state(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        Flags& flags);

    bool repush_trap(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        Flags& flags);

    bool policy_exploration(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID start_state,
        utils::CountdownTimer& timer);

    void backtrack_from_singleton(StateID state, Flags& flags);

    bool backtrack_from_non_singleton(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        const StateID state,
        Flags& flags,
        auto scc);

    bool backtrack_trap(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        const StateID state,
        Flags& flags,
        auto scc);

    void backtrack_solved(const StateID, Flags& flags, auto scc);

    void backtrack_unsolved(const StateID, Flags& flags, auto scc);

    template <bool Convergence>
    UpdateResult value_iteration(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer);
};

template <typename State, typename Action, bool UseInterval>
class TADepthFirstHeuristicSearch : public MDPAlgorithm<State, Action> {
    using Base = typename TADepthFirstHeuristicSearch::MDPAlgorithm;

    using Policy = typename Base::Policy;
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QuotientPolicyPicker = PolicyPicker<QState, QAction>;
    using QuotientOpenList = OpenList<QAction>;

    TADFHSImpl<State, Action, UseInterval> algorithm_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADepthFirstHeuristicSearch(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool interval_comparison,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        std::shared_ptr<QuotientOpenList> open_list);

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    std::unique_ptr<Policy> compute_policy(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    value_t lookup_value(StateID state_id) const;

    bool was_visited(StateID state_id) const;
};

} // namespace trap_aware_dfhs
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#include "probfd/algorithms/trap_aware_dfhs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#endif