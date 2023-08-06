#ifndef PROBFD_ENGINES_TRAP_AWARE_LRTDP_H
#define PROBFD_ENGINES_TRAP_AWARE_LRTDP_H

#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"
#include "probfd/storage/per_state_storage.h"

#include "downward/utils/timer.h"

namespace utils {
class CountdownTimer;
}

namespace probfd {

namespace engine_interfaces {
template <typename>
class SuccessorSampler;
}

namespace engines {

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP) with
/// native trap handling support.
namespace trap_aware_lrtdp {

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

    bool is_solved() const { return (this->info & MASK) == SOLVED; }
    void set_solved() { this->info = (this->info & ~MASK) | SOLVED; }
    bool is_on_trial() const { return (this->info & MARKED_TRIAL); }
    void set_on_trial() { this->info = this->info | MARKED_TRIAL; }
    void clear_trial_flag() { this->info = (this->info & ~MARKED_TRIAL); }
};

} // namespace internal

template <typename State, typename Action, bool UseInterval>
class TALRTDPImpl
    : public heuristic_search::HeuristicSearchBase<
          State,
          quotients::QuotientAction<Action>,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using Base = typename TALRTDPImpl::HeuristicSearchBase;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using Evaluator = typename Base::Evaluator;
    using QuotientPolicyPicker = typename Base::PolicyPicker;
    using QuotientNewStateObserver = typename Base::NewStateObserver;
    using StateInfo = typename Base::StateInfo;

    using QuotientSuccessorSampler =
        engine_interfaces::SuccessorSampler<QAction>;

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
        std::shared_ptr<QuotientNewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        std::shared_ptr<QuotientSuccessorSampler> succ_sampler);

    Interval solve_quotient(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time);

    void print_statistics(std::ostream& out) const;

private:
    bool trial(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        StateID start_state,
        utils::CountdownTimer& timer);

    bool check_and_solve(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        utils::CountdownTimer& timer);

    bool push_to_queue(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        const StateID state,
        Flags& parent_flags);
};

template <typename State, typename Action, bool UseInterval>
class TALRTDP : public MDPEngine<State, Action> {
    using Base = typename TALRTDP::MDPEngine;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using PartialPolicy = typename Base::PartialPolicy;
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using QuotientPolicyPicker =
        engine_interfaces::PolicyPicker<State, QAction>;
    using QuotientNewStateObserver = engine_interfaces::NewStateObserver<State>;
    using QuotientSuccessorSampler =
        engine_interfaces::SuccessorSampler<QAction>;

    TALRTDPImpl<State, Action, UseInterval> engine_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDP(
        std::shared_ptr<QuotientPolicyPicker> policy_chooser,
        std::shared_ptr<QuotientNewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        std::shared_ptr<QuotientSuccessorSampler> succ_sampler);

    Interval
    solve(MDP& mdp, Evaluator& heuristic, param_type<State> s, double max_time)
        override final;

    void print_statistics(std::ostream& out) const override final;
};

} // namespace trap_aware_lrtdp
} // namespace engines
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ENGINES_TRAP_AWARE_LRTDP_H
#include "probfd/engines/trap_aware_lrtdp_impl.h"
#undef GUARD_INCLUDE_PROBFD_ENGINES_TRAP_AWARE_LRTDP_H

#endif // __TRAP_AWARE_LRTDP_H__
