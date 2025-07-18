#ifndef PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#define PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#include "probfd/algorithms/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/distribution.h"

#include "downward/utils/timer.h"

#include <type_traits>
#include <vector>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

namespace probfd::algorithms {
template <typename>
class OpenList;
}

/// Namespace dedicated to the depth-first heuristic search (DFHS) family with
/// native trap handling support.
namespace probfd::algorithms::trap_aware_dfhs {

enum class BacktrackingUpdateType { DISABLED, ON_DEMAND, SINGLE };

namespace internal {

struct Statistics {
    downward::utils::Timer trap_timer = downward::utils::Timer(false);
    unsigned long long iterations = 0;
    unsigned long long traps = 0;
    unsigned long long reexpansions = 0;
    unsigned long long forward_updates = 0;
    unsigned long long backtracking_updates = 0;
    unsigned long long convergence_updates = 0;
    unsigned long long convergence_value_iterations = 0;

    void print(std::ostream& out) const;
    void register_report(ProgressReport& report) const;
};

template <typename Action, bool UseInterval>
struct PerStateInformation
    : public heuristic_search::
          PerStateBaseInformation<Action, true, UseInterval> {
private:
    using Base =
        heuristic_search::PerStateBaseInformation<Action, true, UseInterval>;

public:
    static constexpr uint8_t SOLVED = 1 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 1;
    static constexpr uint8_t MASK = 1 << Base::BITS;

    [[nodiscard]]
    bool is_solved() const
    {
        return this->info & SOLVED || this->is_goal_or_terminal();
    }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
};

} // namespace internal

template <typename, typename, bool>
class TADepthFirstHeuristicSearch;

template <typename State, typename Action, bool UseInterval>
class TADFHSImpl
    : public heuristic_search::HeuristicSearchBase<
          quotients::QuotientState<State, Action>,
          quotients::QuotientAction<Action>,
          internal::PerStateInformation<
              quotients::QuotientAction<Action>,
              UseInterval>> {
    using Base = typename TADFHSImpl::HeuristicSearchBase;

    using AlgorithmValueType = Base::AlgorithmValueType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QHeuristic = typename Base::HeuristicType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientOpenList = OpenList<QAction>;

    template <typename, typename, bool>
    friend class TADepthFirstHeuristicSearch;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
    };

    struct DFSExplorationState {
        StateID state_id;
        uint32_t lowlink;
        std::vector<StateID> successors;

        /// did the value of a descendant change?
        bool value_converged : 1 = true;
        /// were all greedy outside-SCC descendants explored and solved?
        bool solved : 1 = true;
        /// is this state's scc a trap?
        bool is_trap : 1 = true;

        explicit DFSExplorationState(StateID state, int stack_index)
            : state_id(state)
            , lowlink(stack_index)
        {
        }

        bool next_successor();
        StateID get_successor() const;
    };

    struct StackInfo {
        StateID state_id;
        std::optional<QAction> action;

        explicit StackInfo(StateID state_id)
            : state_id(state_id)
        {
        }

        template <size_t i>
        friend auto get(StackInfo& info)
        {
            if constexpr (i == 0) return info.state_id;
            if constexpr (i == 1) return std::views::single(*info.action);
        }

        template <size_t i>
        friend auto get(const StackInfo& info)
        {
            if constexpr (i == 0) return info.state_id;
            if constexpr (i == 1) return std::views::single(*info.action);
        }
    };

    static constexpr uint32_t NEW = std::numeric_limits<uint32_t>::max() - 1;
    static constexpr uint32_t CLOSED = std::numeric_limits<uint32_t>::max();

    const value_t epsilon_;

    // Algorithm parameters
    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool label_solved_;
    const bool reexpand_traps_;

    // Algorithm state
    std::deque<DFSExplorationState> dfs_stack_;
    std::vector<StackInfo> tarjan_stack_;
    storage::StateHashMap<uint32_t> stack_index_;

    std::vector<StateID> visited_states_;

    // Re-used buffer
    std::vector<TransitionTail<QAction>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;
    SuccessorDistribution successor_dist_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADFHSImpl(
        value_t epsilon,
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool label_solved,
        bool reexpand_traps);

    Interval solve_quotient(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        ParamType<QState> qstate,
        ProgressReport& progress,
        downward::utils::Duration max_time);

    void print_statistics(std::ostream& out) const;

private:
    void dfhs_vi_driver(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID state,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer);

    void dfhs_label_driver(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID state,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer);

    bool advance(
        QuotientSystem& quotient,
        DFSExplorationState& einfo,
        StateInfo& state_info);

    bool push_successor(
        QuotientSystem& quotient,
        DFSExplorationState& einfo,
        StateInfo& sinfo,
        downward::utils::CountdownTimer& timer);

    bool initialize(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        DFSExplorationState& einfo,
        StateInfo& state_info);

    void push(StateID state_id);

    bool policy_exploration(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID start_state,
        downward::utils::CountdownTimer& timer);

    bool value_iteration(
        QuotientSystem& quotient,
        const std::ranges::input_range auto& range,
        downward::utils::CountdownTimer& timer);

    std::pair<bool, bool> vi_step(
        QuotientSystem& quotient,
        const std::ranges::input_range auto& range,
        downward::utils::CountdownTimer& timer);
};

template <typename State, typename Action, bool UseInterval>
class TADepthFirstHeuristicSearch : public MDPAlgorithm<State, Action> {
    using Base = typename TADepthFirstHeuristicSearch::MDPAlgorithm;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QuotientPolicyPicker = PolicyPicker<QState, QAction>;

    TADFHSImpl<State, Action, UseInterval> algorithm_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADepthFirstHeuristicSearch(
        value_t epsilon,
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool label_solved,
        bool reexpand_removed_traps);

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time);

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        downward::utils::Duration max_time) override;

    void print_statistics(std::ostream& out) const override;

    [[nodiscard]]
    Interval lookup_bounds(StateID state_id) const;

    [[nodiscard]]
    bool was_visited(StateID state_id) const;
};

} // namespace probfd::algorithms::trap_aware_dfhs

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#include "probfd/algorithms/trap_aware_dfhs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#endif
