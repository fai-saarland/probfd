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

enum class BacktrackingUpdateType { DISABLED, ON_DEMAND, SINGLE };

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
    using Base = heuristic_search::PerStateBaseInformation<Action, true, UseInterval>;

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

    using QEvaluator = typename Base::EvaluatorType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientOpenList = OpenList<QAction>;

    template <typename, typename, bool>
    friend class TADepthFirstHeuristicSearch;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
    };

    struct ExplorationInformation {
        StateID state;
        int lowlink;
        std::vector<StateID> successors;

        /// did the value of a descendant change?
        bool value_converged : 1 = true;
        /// were all greedy outside-SCC descendants explored and solved?
        bool all_solved : 1 = true;
        /// is this state's scc a trap?
        bool is_trap : 1 = true;

        explicit ExplorationInformation(StateID state, int stack_index)
            : state(state)
            , lowlink(stack_index)
        {
        }

        bool next_successor();
        StateID get_successor() const;

        void update(const ExplorationInformation& other);

        void clear();
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

    static constexpr int NEW = -1;
    static constexpr int CLOSED = -2;

    // Algorithm parameters
    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_on_cutoff_;
    const bool label_solved_;
    const bool reexpand_traps_;

    // Algorithm state
    std::deque<ExplorationInformation> queue_;
    std::vector<StackInfo> stack_;
    std::vector<StateID> visited_states_;
    storage::StateHashMap<int> stack_index_;

    bool terminated_ = false;

    // Re-used buffer
    std::vector<Transition<QAction>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;
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
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool terminate_exploration_on_cutoff,
        bool label_solved,
        bool reexpand_traps);

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
        ExplorationInformation& einfo,
        StateID state,
        QAction action,
        const Distribution<StateID>& successor_dist);

    bool advance(QuotientSystem& quotient, ExplorationInformation& einfo);

    bool push_successor(
        QuotientSystem& quotient,
        ExplorationInformation& einfo,
        utils::CountdownTimer& timer);

    bool initialize(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        ExplorationInformation& einfo);

    void push(StateID state_id);

    bool policy_exploration(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID start_state,
        utils::CountdownTimer& timer);

    UpdateResult value_iteration(
        QuotientSystem& quotient,
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
        bool label_solved,
        bool reexpand_removed_traps);

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
    Interval lookup_bounds(StateID state_id) const;

    [[nodiscard]]
    bool was_visited(StateID state_id) const;
};

} // namespace probfd::algorithms::trap_aware_dfhs

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H
#include "probfd/algorithms/trap_aware_dfhs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_DFHS_H

#endif
