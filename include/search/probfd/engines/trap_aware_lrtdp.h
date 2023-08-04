#ifndef PROBFD_ENGINES_TRAP_AWARE_LRTDP_H
#define PROBFD_ENGINES_TRAP_AWARE_LRTDP_H

#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"
#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/timer.h"

namespace probfd {
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

    void print(std::ostream& out) const
    {
        out << "  Trials: " << trials << std::endl;
        out << "  Average trial length: " << ((double)trial_length / trials)
            << std::endl;
        out << "  Bellman backups (trials): " << trial_bellman_backups
            << std::endl;
        out << "  Bellman backups (check&solved): "
            << check_and_solve_bellman_backups << std::endl;
        out << "  Trap removals: " << traps << std::endl;
        out << "  Trap removal time: " << trap_timer << std::endl;
    }

    void register_report(ProgressReport* report) const
    {
        report->register_print([this](std::ostream& out) {
            out << "traps=" << traps << ", trials=" << trials;
        });
    }
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
class TALRTDP;

template <typename State, typename Action, bool UseInterval>
class TALRTDP<State, quotients::QuotientAction<Action>, UseInterval>
    : public heuristic_search::HeuristicSearchBase<
          State,
          quotients::QuotientAction<Action>,
          UseInterval,
          true,
          internal::PerStateInformation> {
    using HeuristicSearchBase = heuristic_search::HeuristicSearchBase<
        State,
        quotients::QuotientAction<Action>,
        UseInterval,
        true,
        internal::PerStateInformation>;

    using QAction = quotients::QuotientAction<Action>;
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;

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

    engine_interfaces::SuccessorSampler<QAction>* sample_;

    Distribution<StateID> selected_transition_;

    std::deque<StateID> current_trial_;

    std::deque<ExplorationInformation> queue_;
    std::deque<StackInfo> stack_;
    storage::StateHashMap<int> stack_index_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDP(
        engine_interfaces::PolicyPicker<State, QAction>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        engine_interfaces::SuccessorSampler<QAction>* succ_sampler)
        : HeuristicSearchBase(
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison)
        , stop_at_consistent_(stop_consistent)
        , reexpand_traps_(reexpand_traps)
        , sample_(succ_sampler)
        , stack_index_(STATE_UNSEEN)
    {
    }

    Interval solve_quotient(
        quotients::QuotientSystem<State, Action>& quotient,
        Evaluator<State>& heuristic,
        param_type<State> s,
        double max_time)
    {
        utils::CountdownTimer timer(max_time);

        const StateID state_id = quotient.get_state_id(s);
        bool terminate = false;
        do {
            terminate = trial(quotient, heuristic, state_id, timer);
            statistics_.trials++;
            assert(state_id == quotient.translate_state_id(state_id));
            this->print_progress();
        } while (!terminate);

        return this->lookup_bounds(state_id);
    }

protected:
    Interval
    do_solve(MDP<State, QAction>&, Evaluator<State>&, param_type<State>, double)
        override
    {
        // FIXME refine class hierarchy as not to inherit this function
        std::cerr << "This function should not be called!" << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
    }

    void print_additional_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    void setup_custom_reports(param_type<State>) override
    {
        this->statistics_.register_report(this->report_);
    }

private:
    bool trial(
        quotients::QuotientSystem<State, Action>& quotient,
        Evaluator<State>& heuristic,
        StateID start_state,
        utils::CountdownTimer& timer)
    {
        using enum TrialTerminationCondition;

        assert(current_trial_.empty());
        assert(selected_transition_.empty());

        ClearGuard guard(current_trial_);
        current_trial_.push_back(start_state);
        for (;;) {
            timer.throw_if_expired();

            StateID stateid = current_trial_.back();
            auto& info = this->get_state_info(stateid);
            if (info.is_solved()) {
                current_trial_.pop_back();
                break;
            }

            statistics_.trial_bellman_backups++;
            const bool changed = this->async_update(
                                         quotient,
                                         heuristic,
                                         stateid,
                                         &selected_transition_)
                                     .value_changed;

            if (selected_transition_.empty()) {
                info.set_solved();
                current_trial_.pop_back();
                break;
            }

            if ((stop_at_consistent_ == CONSISTENT && !changed) ||
                (stop_at_consistent_ == INCONSISTENT && changed) ||
                (stop_at_consistent_ == REVISITED && info.is_on_trial())) {
                selected_transition_.clear();
                break;
            }

            if (stop_at_consistent_ == REVISITED) {
                info.set_on_trial();
            }

            current_trial_.push_back(
                this->sample_state(*sample_, stateid, selected_transition_));
            selected_transition_.clear();
        }

        statistics_.trial_length += current_trial_.size();
        if (stop_at_consistent_ == REVISITED) {
            for (const StateID state : current_trial_) {
                this->get_state_info(state).clear_trial_flag();
            }
        }

        do {
            timer.throw_if_expired();

            if (!check_and_solve(quotient, heuristic, timer)) {
                return false;
            }

            current_trial_.pop_back();
        } while (!current_trial_.empty());

        return true;
    }

    bool check_and_solve(
        quotients::QuotientSystem<State, Action>& quotient,
        Evaluator<State>& heuristic,
        utils::CountdownTimer& timer)
    {
        assert(!this->current_trial_.empty());

        const StateID s =
            quotient.translate_state_id(this->current_trial_.back());
        auto& sinfo = this->get_state_info(s);

        if (sinfo.is_solved()) {
            // was labeled in some prior check_and_solve() invocation
            return true;
        }

        if (Flags flags; !push_to_queue(quotient, heuristic, s, flags)) {
            stack_index_.clear();
            return flags.rv;
        }

        ExplorationInformation* einfo = &queue_.back();

        for (;;) {
            do {
                timer.throw_if_expired();

                const StateID succ =
                    quotient.translate_state_id(einfo->successors.back());
                StateInfo& succ_info = this->get_state_info(succ);
                int& sidx = stack_index_[succ];
                if (sidx == STATE_UNSEEN) {
                    if (succ_info.is_terminal()) {
                        succ_info.set_solved();
                    }
                    if (succ_info.is_solved()) {
                        einfo->flags.update(succ_info);
                    } else if (push_to_queue(
                                   quotient,
                                   heuristic,
                                   succ,
                                   einfo->flags)) {
                        einfo = &queue_.back();
                        continue;
                    }
                    // don't notify_state this state again within this
                    // check_and_solve iteration
                    sidx = STATE_CLOSED;
                } else if (sidx >= 0) {
                    int& sidx2 = stack_index_[einfo->state];
                    if (sidx < sidx2) {
                        sidx2 = sidx;
                        einfo->is_root = false;
                    }
                } else {
                    einfo->flags.update(succ_info);
                }
                einfo->successors.pop_back();
            } while (!einfo->successors.empty());

            do {
                Flags flags = einfo->flags;

                if (einfo->is_root) {
                    const StateID state = einfo->state;
                    const unsigned stack_index = stack_index_[state];
                    std::ranges::subrange scc(
                        std::next(stack_.begin(), stack_index),
                        stack_.end());

                    if (scc.size() == 1) {
                        stack_index_[state] = STATE_CLOSED;
                        stack_.pop_back();
                        if (!einfo->flags.rv) {
                            ++this->statistics_.check_and_solve_bellman_backups;
                            this->async_update(quotient, heuristic, state);
                        } else {
                            this->get_state_info(state).set_solved();
                        }
                    } else {
                        if (einfo->flags.is_trap) {
                            assert(einfo->flags.rv);
                            for (const auto& entry : scc) {
                                stack_index_[entry.state_id] = STATE_CLOSED;
                            }
                            TimerScope scope(statistics_.trap_timer);
                            quotient.build_quotient(scc, *scc.begin());
                            this->get_state_info(state).clear_policy();
                            ++this->statistics_.traps;
                            ++this->statistics_.check_and_solve_bellman_backups;
                            stack_.erase(scc.begin(), scc.end());
                            if (reexpand_traps_) {
                                queue_.pop_back();
                                if (push_to_queue(
                                        quotient,
                                        heuristic,
                                        state,
                                        flags)) {
                                    break;
                                } else {
                                    goto skip_pop;
                                }
                            } else {
                                this->async_update(quotient, heuristic, state);
                                einfo->flags.rv = false;
                            }
                        } else if (einfo->flags.rv) {
                            for (const auto& entry : scc) {
                                stack_index_[entry.state_id] = STATE_CLOSED;
                                this->get_state_info(entry.state_id)
                                    .set_solved();
                            }
                            stack_.erase(scc.begin(), scc.end());
                        } else {
                            for (const auto& entry : scc) {
                                stack_index_[entry.state_id] = STATE_CLOSED;
                                this->async_update(
                                    quotient,
                                    heuristic,
                                    entry.state_id);
                            }
                            stack_.erase(scc.begin(), scc.end());
                        }
                    }

                    flags.is_trap = false;
                }

                queue_.pop_back();

            skip_pop:;

                if (queue_.empty()) {
                    assert(stack_.empty());
                    stack_index_.clear();
                    return sinfo.is_solved();
                }

                timer.throw_if_expired();

                einfo = &queue_.back();

                einfo->flags.update(flags);
                einfo->successors.pop_back();
            } while (einfo->successors.empty());
        }
    }

    bool push_to_queue(
        quotients::QuotientSystem<State, Action>& quotient,
        Evaluator<State>& heuristic,
        const StateID state,
        Flags& parent_flags)
    {
        assert(quotient.translate_state_id(state) == state);
        assert(this->selected_transition_.empty());

        ++this->statistics_.check_and_solve_bellman_backups;

        const auto result = this->async_update(
            quotient,
            heuristic,
            state,
            &this->selected_transition_);

        if (this->selected_transition_.empty()) {
            assert(this->get_state_info(state).is_dead_end());
            parent_flags.rv = parent_flags.rv && !result.value_changed;
            parent_flags.is_trap = false;
            return false;
        }

        if (result.value_changed) {
            parent_flags.rv = false;
            parent_flags.is_trap = false;
            parent_flags.is_dead = false;
            this->selected_transition_.clear();
            return false;
        }

        queue_.emplace_back(state);
        ExplorationInformation& e = queue_.back();
        for (const StateID sel : this->selected_transition_.support()) {
            if (sel != state) {
                e.successors.push_back(sel);
            }
        }

        assert(!e.successors.empty());
        this->selected_transition_.clear();
        e.flags.is_trap = quotient.get_action_cost(*result.policy_action) == 0;
        stack_index_[state] = stack_.size();
        stack_.emplace_back(state, *result.policy_action);
        return true;
    }
};

template <typename State, typename Action, bool UseInterval>
class TALRTDP : public MDPEngine<State, Action> {
    using QAction = quotients::QuotientAction<Action>;

    TALRTDP<State, QAction, UseInterval> engine_;

public:
    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    TALRTDP(
        engine_interfaces::PolicyPicker<State, QAction>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        engine_interfaces::SuccessorSampler<QAction>* succ_sampler)
        : engine_(
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison,
              stop_consistent,
              reexpand_traps,
              succ_sampler)
    {
    }

    Interval solve(
        MDP<State, Action>& mdp,
        Evaluator<State>& heuristic,
        param_type<State> s,
        double max_time) override
    {
        quotients::QuotientSystem<State, Action> quotient(&mdp);
        return engine_.solve_quotient(quotient, heuristic, s, max_time);
    }

    void print_statistics(std::ostream& out) const override
    {
        return engine_.print_statistics(out);
    }
};

} // namespace trap_aware_lrtdp
} // namespace engines
} // namespace probfd

#endif // __TRAP_AWARE_LRTDP_H__
