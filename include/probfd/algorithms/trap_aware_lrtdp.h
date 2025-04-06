#ifndef PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#define PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H

#include "probfd/algorithms/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"
#include "probfd/storage/per_state_storage.h"

#include "downward/utils/timer.h"

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

namespace probfd::algorithms {
template <typename>
class SuccessorSampler;
}

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP) with
/// native trap handling support.
namespace probfd::algorithms::trap_aware_lrtdp {

enum class TrialTerminationCondition {
    TERMINAL,
    CONSISTENT,
    INCONSISTENT,
    REVISITED,
};

namespace internal {

struct Statistics {
    unsigned long long trials = 0;
    unsigned long long trial_bellman_backups = 0;
    unsigned long long check_and_solve_bellman_backups = 0;
    unsigned long long traps = 0;
    unsigned long long trial_length = 0;
    downward::utils::Timer trap_timer = downward::utils::Timer(false);

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
    static constexpr uint8_t MARKED_TRIAL = 1 << Base::BITS;
    static constexpr uint8_t SOLVED = 2 << Base::BITS;
    static constexpr uint8_t BITS = Base::BITS + 2;
    static constexpr uint8_t MASK = 3 << Base::BITS;

    bool is_solved() const
    {
        return (this->info & MASK) == SOLVED || this->is_goal_or_terminal();
    }

    bool is_on_trial() const { return (this->info & MARKED_TRIAL); }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
    void set_on_trial() { this->info = this->info | MARKED_TRIAL; }

    void clear_trial_flag() { this->info = (this->info & ~MARKED_TRIAL); }
};

} // namespace internal

template <typename, typename, bool>
class TALRTDP;

template <typename State, typename Action, bool UseInterval>
class TALRTDPImpl
    : public heuristic_search::HeuristicSearchBase<
          quotients::QuotientState<State, Action>,
          quotients::QuotientAction<Action>,
          internal::PerStateInformation<
              quotients::QuotientAction<Action>,
              UseInterval>> {
    using Base = typename TALRTDPImpl::HeuristicSearchBase;

    using AlgorithmValueType = typename Base::AlgorithmValueType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QHeuristic = typename Base::HeuristicType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientSuccessorSampler = SuccessorSampler<QAction>;

    template <typename, typename, bool>
    friend class TALRTDP;

    struct DFSExplorationState {
        explicit DFSExplorationState(StateID state_id, uint32_t stack_index)
            : state(state_id)
            , lowlink(stack_index)
        {
        }

        StateID state;
        std::vector<StateID> successors;

        uint32_t lowlink;

        bool is_trap : 1 = true;
        bool rv : 1 = true;

        bool next_successor();
        [[nodiscard]]
        StateID get_successor() const;
    };

    struct StackInfo {
        StateID state_id;
        std::vector<QAction> aops;

        explicit StackInfo(StateID state_id)
            : state_id(state_id)
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

    static constexpr uint32_t STATE_UNSEEN =
        std::numeric_limits<uint32_t>::max();
    static constexpr uint32_t STATE_CLOSED =
        std::numeric_limits<uint32_t>::max() - 1;

    const value_t epsilon_;

    // Algorithm parameters
    const TrialTerminationCondition stop_at_consistent_;
    const bool reexpand_traps_;
    const std::shared_ptr<QuotientSuccessorSampler> sample_;

    // Algorithm state
    std::deque<DFSExplorationState> dfs_stack_;
    std::deque<StackInfo> tarjan_stack_;
    storage::StateHashMap<uint32_t> stack_index_;

    std::deque<StateID> current_trial_;

    internal::Statistics statistics_;

    // Buffer
    std::vector<TransitionTail<QAction>> transitions_;
    std::vector<AlgorithmValueType> qvalues_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDPImpl(
        value_t epsilon,
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        std::shared_ptr<QuotientSuccessorSampler> succ_sampler);

    Interval solve_quotient(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        ParamType<QState> state,
        ProgressReport& progress,
        double max_time);

    void print_statistics(std::ostream& out) const;

private:
    bool trial(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID start_state,
        downward::utils::CountdownTimer& timer);

    bool check_and_solve(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID state_id,
        downward::utils::CountdownTimer& timer);

    bool push_successor(
        QuotientSystem& quotient,
        DFSExplorationState& einfo,
        downward::utils::CountdownTimer& timer);

    void push(StateID state);

    bool initialize(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        StateID state,
        StateInfo& state_info,
        DFSExplorationState& e_info);

    void do_non_tip_bellman_update(
        QuotientSystem& quotient,
        const QState& state,
        StateInfo& info);
};

template <typename State, typename Action, bool UseInterval>
class TALRTDP : public MDPAlgorithm<State, Action> {
    using Base = typename TALRTDP::MDPAlgorithm;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;
    using PolicyType = typename Base::PolicyType;

    using QuotientPolicyPicker = PolicyPicker<QState, QAction>;
    using QuotientSuccessorSampler = SuccessorSampler<QAction>;

    TALRTDPImpl<State, Action, UseInterval> algorithm_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDP(
        value_t epsilon,
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        std::shared_ptr<QuotientSuccessorSampler> succ_sampler);

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> s,
        ProgressReport progress,
        double max_time);

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> s,
        ProgressReport progress,
        double max_time) final;

    void print_statistics(std::ostream& out) const final;
};

} // namespace probfd::algorithms::trap_aware_lrtdp

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#include "probfd/algorithms/trap_aware_lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H

#endif // PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
