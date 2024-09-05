#ifndef PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#define PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#include "probfd/algorithms/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/distribution.h"

#include "downward/utils/timer.h"

#include <type_traits>
#include <vector>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

namespace probfd::algorithms {
template <typename>
class OpenList;
}

/// Namespace dedicated to the depth-first heuristic search (DFHS) family with
/// native trap handling support.
namespace probfd::algorithms::trap_aware_dfhs {

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

template <typename Action, bool UseInterval>
struct PerStateInformation
    : public heuristic_search::
          PerStateBaseInformation<Action, true, UseInterval> {
private:
    using Base = PerStateInformation::PerStateBaseInformation;

public:
    static constexpr uint8_t SOLVED = 1 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 1;
    static constexpr uint8_t MASK = 1 << Base::BITS;

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED || this->is_terminal();
    }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
};

} // namespace internal

template <typename State, typename Action, bool UseInterval>
class TADFHSImpl
    : public heuristic_search::HeuristicSearchBase<
          quotients::QuotientState<State, Action>,
          quotients::QuotientAction<Action>,
          internal::PerStateInformation<
              quotients::QuotientAction<Action>,
              UseInterval>> {
    using Base = typename TADFHSImpl::HeuristicSearchBase;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QEvaluator = typename Base::EvaluatorType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientOpenList = OpenList<QAction>;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
    };

    struct Flags {
        /// was the exploration cut off?
        bool complete = true;
        /// were all reached outside-SCC states marked solved?
        bool all_solved = true;
        /// are there any outside-SCC states reachable, and do all transitions
        /// within the SCC generate 0-cost?
        bool is_trap = true;

        void clear()
        {
            complete = true;
            all_solved = true;
            is_trap = true;
        }

        void update(const Flags& flags)
        {
            complete = complete && flags.complete;
            all_solved = all_solved && flags.all_solved;
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

        bool next_successor();
        [[nodiscard]]
        StateID get_successor() const;
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

    // Algorithm parameters
    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool mark_solved_;
    const bool reexpand_removed_traps_;
    const std::shared_ptr<QuotientOpenList> open_list_;

    // Algorithm state
    std::deque<ExplorationInformation> queue_;
    std::vector<StackInfo> stack_;
    std::vector<StateID> visited_states_;
    storage::StateHashMap<int> stack_index_;

    bool terminated_ = false;

    // Re-used buffer
    Distribution<StateID> transition_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADFHSImpl(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
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
        StateID state,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    void dfhs_label_driver(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    void enqueue(
        QuotientSystem& quotient,
        StateID state,
        QAction action,
        const Distribution<StateID>& successor_dist);

    bool push_successor(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        ExplorationInformation einfo,
        utils::CountdownTimer& timer);

    bool push_state(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        StateInfo& state_info,
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
        StateID state,
        Flags& flags,
        auto scc);

    bool backtrack_trap(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        Flags& flags,
        auto scc);

    void backtrack_solved(StateID, Flags& flags, auto scc);
    void backtrack_unsolved(StateID, Flags& flags, auto scc);

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

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;

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
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        std::shared_ptr<QuotientOpenList> open_list);

    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    [[nodiscard]]
    value_t lookup_value(StateID state_id) const;

    [[nodiscard]]
    bool was_visited(StateID state_id) const;
};

} // namespace probfd::algorithms::trap_aware_dfhs

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#include "probfd/algorithms/trap_aware_dfhs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#endif