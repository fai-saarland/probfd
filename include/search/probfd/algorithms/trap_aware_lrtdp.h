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

template <typename StateInfo>
struct PerStateInformation : public StateInfo {
    static constexpr uint8_t MARKED_TRIAL = (1 << StateInfo::BITS);
    static constexpr uint8_t SOLVED = (2 << StateInfo::BITS);
    static constexpr uint8_t BITS = StateInfo::BITS + 2;
    static constexpr uint8_t MASK = (3 << StateInfo::BITS);

    [[nodiscard]]
    bool is_solved() const
    {
        return (this->info & MASK) == SOLVED;
    }

    [[nodiscard]]
    bool is_on_trial() const
    {
        return (this->info & MARKED_TRIAL);
    }

    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
    void set_on_trial() { this->info = this->info | MARKED_TRIAL; }

    void clear_trial_flag() { this->info = (this->info & ~MARKED_TRIAL); }
};

} // namespace internal

template <typename State, typename Action, bool UseInterval>
class TALRTDPImpl
    : public heuristic_search::HeuristicSearchBaseExt<
          quotients::QuotientState<State, Action>,
          quotients::QuotientAction<Action>,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using Base = typename TALRTDPImpl::HeuristicSearchBase;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QEvaluator = typename Base::EvaluatorType;
    using QuotientPolicyPicker = typename Base::PolicyPickerType;
    using StateInfo = typename Base::StateInfo;

    using QuotientSuccessorSampler = SuccessorSampler<QAction>;

    struct Flags {
        bool is_dead = true;
        bool is_trap = true;
        bool rv = true;

        void clear()
        {
            is_dead = true;
            is_trap = true;
            rv = true;
        }

        void update(const Flags& flags)
        {
            is_trap = is_trap && flags.is_trap;
            is_dead = is_dead && flags.is_dead;
            rv = rv && flags.rv;
        }

        void update(const StateInfo& succ_info)
        {
            is_trap = false;
            is_dead = is_dead && succ_info.is_dead_end();
            rv = rv && succ_info.is_solved();
        }
    };

    struct ExplorationInformation {
        explicit ExplorationInformation(StateID state_id)
            : state(state_id)
        {
        }

        StateID state;
        std::vector<StateID> successors;
        bool is_root = true;
        Flags flags;

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

    const TrialTerminationCondition stop_at_consistent_;
    const bool reexpand_traps_;

    std::shared_ptr<QuotientSuccessorSampler> sample_;

    std::deque<StateID> current_trial_;

    std::deque<ExplorationInformation> queue_;
    std::deque<StackInfo> stack_;
    storage::StateHashMap<int> stack_index_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDPImpl(
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
        QEvaluator& heuristic,
        ExplorationInformation& einfo,
        utils::CountdownTimer& timer);

    bool push(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        StateID state,
        Flags& parent_flags);
};

template <typename State, typename Action, bool UseInterval>
class TALRTDP : public MDPAlgorithm<State, Action> {
    using Base = typename TALRTDP::MDPAlgorithm;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;

    using QuotientPolicyPicker = PolicyPicker<QState, QAction>;
    using QuotientSuccessorSampler = SuccessorSampler<QAction>;

    TALRTDPImpl<State, Action, UseInterval> algorithm_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDP(
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

    void print_statistics(std::ostream& out) const final;
};

} // namespace probfd::algorithms::trap_aware_lrtdp

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
#include "probfd/algorithms/trap_aware_lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H

#endif // PROBFD_ALGORITHMS_TRAP_AWARE_LRTDP_H
