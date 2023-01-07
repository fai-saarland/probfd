#ifndef MDPS_ENGINES_TRAP_AWARE_LRTDP_H
#define MDPS_ENGINES_TRAP_AWARE_LRTDP_H

#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotient_system/quotient_system.h"
#include "probfd/storage/per_state_storage.h"

#include "utils/timer.h"

#include <iostream>

namespace probfd {
namespace engines {

/// Namespace dedicated to labelled real-time dynamic programming (LRTDP) with
/// native trap handling support.
namespace trap_aware_lrtdp {

enum class TrialTerminationCondition {
    Disabled = 0,
    Consistent = 1,
    Inconsistent = 2,
    DoublyVisited = 3,
};

namespace internal {

struct Statistics {
    unsigned long long trials = 0;
    unsigned long long trial_bellman_backups = 0;
    unsigned long long check_and_solve_bellman_backups = 0;
    unsigned long long traps = 0;
    unsigned long long trial_length = 0;
    utils::Timer trap_timer;

    explicit Statistics() { trap_timer.stop(); }

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

template <typename StateT, typename QActionT, typename BoundsType>
class LRTDP
    : public heuristic_search::HeuristicSearchBase<
          StateT,
          QActionT,
          BoundsType,
          std::true_type,
          internal::PerStateInformation> {

    static constexpr int STATE_UNSEEN = -1;
    static constexpr int STATE_CLOSED = -2;

public:
    using State = StateT;
    using Action = typename quotient_system::unwrap_action_type<QActionT>::type;
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using QAction = QActionT;
    using HeuristicSearchBase = heuristic_search::HeuristicSearchBase<
        State,
        QAction,
        BoundsType,
        std::true_type,
        internal::PerStateInformation>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;

    /**
     * @brief Constructs a trap-aware LRTDP solver object.
     */
    LRTDP(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<QAction>* action_id_map,
        engine_interfaces::RewardFunction<State, QAction>* reward_function,
        engine_interfaces::TransitionGenerator<QAction>* transition_generator,
        engine_interfaces::PolicyPicker<QAction>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        engine_interfaces::StateEvaluator<State>* value_init,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        QuotientSystem* quotient,
        TrialTerminationCondition stop_consistent,
        bool reexpand_traps,
        engine_interfaces::TransitionSampler<QAction>* succ_sampler)
        : HeuristicSearchBase(
              state_id_map,
              action_id_map,
              reward_function,
              transition_generator,
              policy_chooser,
              new_state_handler,
              value_init,
              report,
              interval_comparison,
              stable_policy)
        , quotient_(quotient)
        , stop_at_consistent_(stop_consistent)
        , reexpand_traps_(reexpand_traps)
        , sample_(succ_sampler)
        , selected_transition_()
        , current_trial_()
        , stack_index_(STATE_UNSEEN)
        , statistics_()
    {
    }

    /**
     * @copydoc MDPEngineInterface::solve(const State& state)
     */
    virtual value_type::value_t solve(const State& s) override
    {
        this->initialize_report(s);
        const StateID state_id = this->get_state_id(s);
        bool terminate = false;
        do {
            terminate = trial(state_id);
            statistics_.trials++;
            assert(state_id == quotient_->translate_state_id(state_id));
            this->report(state_id);
        } while (!terminate);

        return this->get_value(state_id);
    }

    /**
     * @copydoc MDPEngineInterface::print_statistics(std::ostream& out) const
     */
    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
        HeuristicSearchBase::print_statistics(out);
    }

protected:
    virtual void setup_custom_reports(const State&) override
    {
        this->statistics_.register_report(this->report_);
    }

private:
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
        explicit ExplorationInformation(const StateID& state_id)
            : state(state_id)
        {
        }

        StateID state;
        std::vector<StateID> successors;
        bool is_root = true;
        Flags flags;
    };

    bool trial(const StateID& start_state)
    {
        assert(current_trial_.empty());
        assert(selected_transition_.empty());

        current_trial_.push_back(start_state);
        while (true) {
            StateID stateid = current_trial_.back();
            auto& info = this->get_state_info(stateid);
            if (info.is_solved()) {
                current_trial_.pop_back();
                break;
            }

            statistics_.trial_bellman_backups++;
            const bool changed =
                this->async_update(stateid, nullptr, &selected_transition_)
                    .first;

            if (selected_transition_.empty()) {
                info.set_solved();
                current_trial_.pop_back();
                break;
            }

            if ((stop_at_consistent_ == TrialTerminationCondition::Consistent &&
                 !changed) ||
                (stop_at_consistent_ ==
                     TrialTerminationCondition::Inconsistent &&
                 changed) ||
                (stop_at_consistent_ ==
                     TrialTerminationCondition::DoublyVisited &&
                 info.is_on_trial())) {
                selected_transition_.clear();
                break;
            }

            if (stop_at_consistent_ ==
                TrialTerminationCondition::DoublyVisited) {
                info.set_on_trial();
            }

            current_trial_.push_back(sample_->operator()(
                stateid,
                this->get_policy(stateid),
                selected_transition_,
                *this));
            selected_transition_.clear();
        }

        statistics_.trial_length += current_trial_.size();
        if (stop_at_consistent_ == TrialTerminationCondition::DoublyVisited) {
            for (auto it = current_trial_.begin(); it != current_trial_.end();
                 ++it) {
                auto& info = this->get_state_info(*it);
                assert(info.is_on_trial());
                info.clear_trial_flag();
            }
        }

        bool terminate = true;
        while (!current_trial_.empty()) {
            if (!check_and_solve()) {
                terminate = false;
                break;
            }
            current_trial_.pop_back();
        }

        current_trial_.clear();
        return terminate;
    }

    bool check_and_solve()
    {
        assert(!this->current_trial_.empty());

        Flags flags;
        {
            const StateID s =
                quotient_->translate_state_id(this->current_trial_.back());
            if (this->get_state_info(s).is_solved()) {
                // was labeled in some prior check_and_solve() invocation
                return true;
            }
            if (!push_to_queue(s, flags)) {
                stack_index_.clear();
                return flags.rv;
            }
        }

        do {
            ExplorationInformation& einfo = queue_.back();
            einfo.flags.update(flags);
            bool backtrack = true;
            while (!einfo.successors.empty()) {
                const StateID succ =
                    quotient_->translate_state_id(einfo.successors.back());
                StateInfo& succ_info = this->get_state_info(succ);
                int& sidx = stack_index_[succ];
                if (sidx == STATE_UNSEEN) {
                    if (succ_info.is_terminal()) {
                        succ_info.set_solved();
                    }
                    if (succ_info.is_solved()) {
                        einfo.flags.update(succ_info);
                    } else if (push_to_queue(succ, einfo.flags)) {
                        flags.clear();
                        backtrack = false;
                        break;
                    }
                    // don't touch this state again within this check_and_solve
                    // iteration
                    sidx = STATE_CLOSED;
                } else if (sidx >= 0) {
                    int& sidx2 = stack_index_[einfo.state];
                    if (sidx < sidx2) {
                        sidx2 = sidx;
                        einfo.is_root = false;
                    }
                } else {
                    einfo.flags.update(succ_info);
                }
                einfo.successors.pop_back();
            }

            if (backtrack) {
                if (einfo.is_root) {
                    const unsigned stack_index = stack_index_[einfo.state];
                    const unsigned scc_size = stack_.size() - stack_index;
                    if (scc_size == 1) {
                        assert(stack_.front() == einfo.state);
                        stack_index_[einfo.state] = STATE_CLOSED;
                        stack_.pop_front();
                        if (!einfo.flags.rv) {
                            ++this->statistics_.check_and_solve_bellman_backups;
                            this->async_update(einfo.state);
                        } else {
                            StateInfo& info = this->get_state_info(einfo.state);
                            info.set_solved();
                        }
                    } else {
                        auto scc_end = std::next(stack_.begin(), scc_size);
                        if (einfo.flags.is_trap) {
                            assert(einfo.flags.rv);
                            StateID minstate = einfo.state;
                            for (auto it = stack_.begin(); it != scc_end;
                                 ++it) {
                                stack_index_[*it] = STATE_CLOSED;
                                if (*it < minstate) {
                                    minstate = *it;
                                }
                            }
                            statistics_.trap_timer.resume();
                            quotient_->build_quotient(
                                stack_.begin(),
                                scc_end,
                                minstate);
                            this->get_state_info(minstate).set_policy(ActionID::undefined);
                            ++this->statistics_.traps;
                            ++this->statistics_.check_and_solve_bellman_backups;
                            stack_.erase(stack_.begin(), scc_end);
                            if (reexpand_traps_) {
                                queue_.pop_back();
                                flags.clear();
                                if (!push_to_queue(minstate, flags)) {
                                    flags.is_trap = false;
                                }
                                statistics_.trap_timer.stop();
                                continue;
                            } else {
                                this->async_update(minstate);
                                einfo.flags.rv = false;
                                statistics_.trap_timer.stop();
                            }
                        } else if (einfo.flags.rv) {
                            for (auto it = stack_.begin(); it != scc_end;
                                 ++it) {
                                stack_index_[*it] = STATE_CLOSED;
                                this->get_state_info(*it).set_solved();
                            }
                            stack_.erase(stack_.begin(), scc_end);
                        } else {
                            for (auto it = stack_.begin(); it != scc_end;
                                 ++it) {
                                stack_index_[*it] = STATE_CLOSED;
                                this->async_update(*it);
                            }
                            stack_.erase(stack_.begin(), scc_end);
                        }
                    }
                    einfo.flags.is_trap = false;
                }
                flags = einfo.flags;
                queue_.pop_back();
            }
        } while (!queue_.empty());

        assert(queue_.empty());
        assert(stack_.empty());

        stack_index_.clear();
        return this->get_state_info(this->current_trial_.back()).is_solved();
    }

    bool push_to_queue(const StateID state, Flags& parent_flags)
    {
        assert(this->quotient_->translate_state_id(state) == state);
        assert(this->selected_transition_.empty());

        ++this->statistics_.check_and_solve_bellman_backups;

        ActionID greedy_action;
        const bool value_changed = this->async_update(
                                           state,
                                           &greedy_action,
                                           &this->selected_transition_)
                                       .first;

        if (this->selected_transition_.empty()) {
            assert(this->get_state_info(state).is_dead_end());
            parent_flags.rv = parent_flags.rv && !value_changed;
            parent_flags.is_trap = false;
            return false;
        }

        if (value_changed) {
            parent_flags.rv = false;
            parent_flags.is_trap = false;
            parent_flags.is_dead = false;
            this->selected_transition_.clear();
            return false;
        }

        queue_.emplace_back(state);
        ExplorationInformation& e = queue_.back();
        for (auto it = this->selected_transition_.begin();
             it != this->selected_transition_.end();
             ++it) {
            if (it->element != state) {
                e.successors.push_back(it->element);
            }
        }

        assert(!e.successors.empty());
        this->selected_transition_.clear();
        e.flags.is_trap = this->get_action_reward(
                              state,
                              this->lookup_action(state, greedy_action)) == 0;
        stack_index_[state] = stack_.size();
        stack_.push_front(state);
        return true;
    }

    QuotientSystem* quotient_;

    const TrialTerminationCondition stop_at_consistent_;
    const bool reexpand_traps_;

    engine_interfaces::TransitionSampler<QAction>* sample_;

    Distribution<StateID> selected_transition_;

    std::deque<StateID> current_trial_;

    std::deque<ExplorationInformation> queue_;
    std::deque<StateID> stack_;
    storage::StateHashMap<int> stack_index_;

    internal::Statistics statistics_;
};

} // namespace trap_aware_lrtdp
} // namespace engines
} // namespace probfd

#endif // __TRAP_AWARE_LRTDP_H__
