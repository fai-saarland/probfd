#ifndef PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#define PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H

#include "probfd/algorithms/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"
#include "probfd/storage/per_state_storage.h"

#include "downward/utils/timer.h"

// Forward Declarations
namespace utils {
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
    utils::Timer trap_timer = utils::Timer(true);

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

    using AlgorithmValueType = Base::AlgorithmValueType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QEvaluator = typename Base::EvaluatorType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientSuccessorSampler = SuccessorSampler<QAction>;

    template <typename, typename, bool>
    friend class TALRTDP;

    struct ExplorationInformation {
        explicit ExplorationInformation(StateID state_id)
            : state(state_id)
        {
        }

        StateID state;
        std::vector<StateID> successors;
        bool is_root : 1 = true;
        bool is_trap : 1 = true;
        bool rv : 1 = true;

        bool next_successor();
        [[nodiscard]]
        StateID get_successor() const;

        void update(const ExplorationInformation& backtracked)
        {
            is_trap = is_trap && backtracked.is_trap;
            rv = rv && backtracked.rv;
        }

        void update(const StateInfo& succ_info)
        {
            is_trap = false;
            rv = rv && succ_info.is_solved();
        }

        void clear()
        {
            is_trap = true;
            rv = true;
        }
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

    static constexpr int STATE_UNSEEN = -1;
    static constexpr int STATE_CLOSED = -2;

    const value_t epsilon_;

    // Algorithm parameters
    const TrialTerminationCondition stop_at_consistent_;
    const bool reexpand_traps_;
    const std::shared_ptr<QuotientSuccessorSampler> sample_;

    // Algorithm state
    std::deque<ExplorationInformation> queue_;
    std::deque<StackInfo> stack_;
    storage::StateHashMap<int> stack_index_;

    std::deque<StateID> current_trial_;

    internal::Statistics statistics_;

    // Buffer
    std::vector<Transition<QAction>> transitions_;
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
        QEvaluator& heuristic,
        param_type<QState> state,
        ProgressReport& progress,
        double max_time);

    void print_statistics(std::ostream& out) const;

private:
    bool trial(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID start_state,
        utils::CountdownTimer& timer);

    bool check_and_solve(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        utils::CountdownTimer& timer);

    bool push_successor(
        QuotientSystem& quotient,
        ExplorationInformation& einfo,
        utils::CountdownTimer& timer);

    void push(StateID state);

    bool initialize(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        StateInfo& state_info,
        ExplorationInformation& e_info);
};

template <typename State, typename Action, bool UseInterval>
class TALRTDP : public MDPAlgorithm<State, Action> {
    using Base = typename TALRTDP::MDPAlgorithm;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
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
        EvaluatorType& heuristic,
        param_type<State> s,
        ProgressReport progress,
        double max_time) final;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> s,
        ProgressReport progress,
        double max_time) final;

    void print_statistics(std::ostream& out) const final;
};

} // namespace probfd::algorithms::trap_aware_lrtdp

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#include "probfd/algorithms/trap_aware_lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H

#endif // PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
