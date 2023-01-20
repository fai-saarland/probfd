#pragma once

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotient_system/quotient_system.h"

#include "utils/timer.h"

#include <cassert>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to the depth-first heuristic search (DFHS) family with
/// native trap handling support.
namespace trap_aware_dfhs {

enum class BacktrackingUpdateType {
    Disabled,
    OnDemand,
    Single,
    UntilConvergence,
};

namespace internal {

struct Statistics {
    explicit Statistics() { trap_timer.stop(); }

    void print(std::ostream& out) const
    {
        out << "  Iterations: " << iterations << std::endl;
        out << "  Traps: " << traps << std::endl;
        out << "  Bellman backups (forward): " << fw_updates << std::endl;
        out << "  Bellman backups (backward): " << bw_updates << std::endl;
        out << "  State re-expansions: " << reexpansions << std::endl;
        out << "  Trap removal time: " << trap_timer << std::endl;
    }

    void register_report(ProgressReport* report) const
    {
        report->register_print([this](std::ostream& out) {
            out << "iteration=" << iterations << ", traps=" << traps;
        });
    }

    utils::Timer trap_timer;
    unsigned long long iterations = 0;
    unsigned long long traps = 0;
    unsigned long long reexpansions = 0;
    unsigned long long fw_updates = 0;
    unsigned long long bw_updates = 0;
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

template <typename State, typename QAction, bool Interval>
class DepthFirstHeuristicSearch
    : public heuristic_search::HeuristicSearchBase<
          State,
          QAction,
          Interval,
          true,
          internal::PerStateInformation> {
    static constexpr int STATE_UNSEEN = -1;
    static constexpr int STATE_CLOSED = -2;

public:
    using Action = typename quotient_system::unwrap_qaction_type<QAction>;
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using HeuristicSearchBase = heuristic_search::HeuristicSearchBase<
        State,
        QAction,
        Interval,
        true,
        internal::PerStateInformation>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;

    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    DepthFirstHeuristicSearch(
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
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        engine_interfaces::OpenList<QAction>* open_list)
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
        , forward_updates_(forward_updates)
        , expand_tip_states_(expand_tip_states)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , terminate_exploration_(stop_exploration_inconsistent)
        , value_iteration_(value_iteration)
        , mark_solved_(mark_solved)
        , backtrack_update_type_(backtrack_update_type)
        , reexpand_removed_traps_(reexpand_removed_traps)
        , open_list_(open_list)
        , terminated_(false)
        , last_value_changed_(false)
        , last_policy_changed_(false)
        , stack_index_(STATE_UNSEEN)
    {
    }

    /**
     * @copydoc MDPEngineInterface::solve(const State& state)
     */
    virtual value_type::value_t solve(const State& qstate) override
    {
        this->initialize_report(qstate);
        StateID state_id = this->get_state_id(qstate);
        if (value_iteration_) {
            dfhs_vi_driver(state_id);
        } else {
            dfhs_label_driver(state_id);
        }
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
        statistics_.register_report(this->report_);
    }

private:
    struct Flags {
        /// was the exploration cut off?
        bool complete = true;
        /// were all reached outside-SCC states marked solved?
        bool all_solved = true;
        /// were all reached outside-SCC states marked dead end?
        bool dead = true;
        /// are there any outside-SCC states reachable, and do all transitions
        /// within the SCC generate 0-reward?
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
        explicit ExplorationInformation(StateID state, int stack_index)
            : state(state)
            , backlink(stack_index)
        {
        }
        StateID state;
        std::vector<StateID> successors;
        Flags flags;
        int backlink;
    };

    void dfhs_vi_driver(const StateID state)
    {
        std::pair<bool, bool> vi_res(true, true);
        do {
            const bool is_complete = policy_exploration(state);
            if (is_complete) {
                vi_res = value_iteration<false>(
                    visited_states_.begin(),
                    visited_states_.end());
            }
            visited_states_.clear();
            ++statistics_.iterations;
            this->report(state);
        } while (vi_res.first || vi_res.second);
    }

    void dfhs_label_driver(const StateID state)
    {
        bool is_complete = false;
        do {
            is_complete = policy_exploration(state) &&
                          this->get_state_info(state).is_solved();
            visited_states_.clear();
            ++statistics_.iterations;
            this->report(state);
        } while (!is_complete);
    }

    void enqueue(const StateID& state)
    {
        queue_.emplace_back(state, stack_.size());
        stack_index_[state] = stack_.size();
        stack_.push_back(state);

        ExplorationInformation& info = queue_.back();
        info.successors.reserve(transition_.size());

        if (open_list_ == nullptr) {
            for (auto it = transition_.begin(); it != transition_.end(); ++it) {
                if (it->element != state) {
                    info.successors.push_back(it->element);
                }
            }
        } else {
            const QAction a = this->get_policy(state);
            for (auto it = transition_.begin(); it != transition_.end(); ++it) {
                if (it->element != state) {
                    open_list_->push(state, a, it->probability, it->element);
                }
            }
            info.successors.resize(open_list_->size(), StateID::undefined);
            int i = open_list_->size() - 1;
            while (!open_list_->empty()) {
                info.successors[i] = open_list_->pop();
                --i;
            }
        }

        assert(!info.successors.empty());
        transition_.clear();
        info.flags.is_trap = has_zero_reward_;
    }

    bool push_state(const StateID& state, StateInfo& state_info, Flags& flags)
    {
        assert(!terminated_);
        last_value_changed_ = false;
        last_policy_changed_ = false;
        assert(!state_info.is_solved() && !state_info.is_terminal());

        const bool tip = state_info.is_on_fringe();
        if (tip || forward_updates_) {
            ++statistics_.fw_updates;
            ActionID greedy_action;
            last_value_changed_ =
                this->async_update(state, &greedy_action, &transition_).first;
            flags.all_solved = flags.all_solved && !last_value_changed_;
            const bool cutoff = (!expand_tip_states_ && tip) ||
                                (cutoff_inconsistent_ && last_value_changed_);
            terminated_ = terminate_exploration_ && cutoff;
            if (transition_.empty()) {
                assert(state_info.is_dead_end());
                flags.is_trap = false;
                return false;
            }
            if (cutoff) {
                transition_.clear();
                flags.complete = false;
                return false;
            }
            has_zero_reward_ =
                this->get_action_reward(
                    state,
                    this->lookup_action(state, greedy_action)) == 0;
        } else {
            QAction action = this->get_policy(state);
            this->generate_successors(state, action, transition_);
            has_zero_reward_ = this->get_action_reward(state, action) == 0;
        }

        enqueue(state);
        return true;
    }

    bool push_state(const StateID& state, Flags& flags)
    {
        StateInfo& state_info = this->get_state_info(state);
        if (state_info.is_terminal() || state_info.is_solved()) {
            state_info.set_solved();
            flags.update(state_info);
            return false;
        }

        return push_state(state, state_info, flags);
    }

    bool repush_trap(const StateID& state, Flags& flags)
    {
        last_value_changed_ = false;
        last_policy_changed_ = false;
        flags.clear();
        ++statistics_.fw_updates;

        ActionID greedy_action;
        last_value_changed_ =
            this->async_update(state, &greedy_action, &transition_).first;
        flags.all_solved = !last_value_changed_;
        const bool cutoff = !reexpand_removed_traps_ ||
                            (cutoff_inconsistent_ && last_value_changed_);
        terminated_ = terminated_ || (terminate_exploration_ && cutoff);

        if (transition_.empty()) {
            flags.is_trap = false;
            return false;
        }

        if (cutoff) {
            transition_.clear();
            flags.complete = false;
            return false;
        }

        if (!queue_.empty()) {
            queue_.back().flags.update(flags);
        }

        flags.clear();
        has_zero_reward_ = this->get_action_reward(
                               state,
                               this->lookup_action(state, greedy_action)) == 0;
        enqueue(state);
        return true;
    }

    bool policy_exploration(const StateID& start_state)
    {
        assert(visited_states_.empty());
        terminated_ = false;
        Flags flags;
        if (!push_state(start_state, flags)) {
            return flags.complete;
        }
        flags.clear();

        int recursiveBacklink = std::numeric_limits<int>::max();
        do {
            ExplorationInformation& einfo = queue_.back();
            einfo.backlink = std::min(recursiveBacklink, einfo.backlink);
            einfo.flags.update(flags);
            bool backtrack = true;
            while (!terminated_ && !einfo.successors.empty()) {
                const StateID succ =
                    quotient_->translate_state_id(einfo.successors.back());
                einfo.successors.pop_back();

                int& succ_status = stack_index_[succ];
                if (succ_status == STATE_UNSEEN) {
                    // expand state (either not expanded before, or last
                    // value change was before pushing einfo.state)
                    StateInfo& succ_info = this->get_state_info(succ);
                    if (succ_info.is_terminal() || succ_info.is_solved()) {
                        succ_info.set_solved();
                        einfo.flags.update(succ_info);
                    } else if (push_state(succ, succ_info, einfo.flags)) {
                        backtrack = false;
                        flags.clear();
                        recursiveBacklink = std::numeric_limits<int>::max();
                        break;
                    } else if (mark_solved_) {
                        einfo.flags.all_solved = false;
                    }
                    succ_status = STATE_CLOSED;
                } else if (succ_status == STATE_CLOSED) {
                    const StateInfo& info = this->get_state_info(succ);
                    einfo.flags.update(info);
                    if (mark_solved_) {
                        einfo.flags.all_solved =
                            einfo.flags.all_solved &&
                            this->get_state_info(succ).is_solved();
                    }
                } else {
                    // is on stack
                    einfo.backlink = std::min(einfo.backlink, succ_status);
                }
            }

            if (backtrack) {
                const StateID state = einfo.state;
                const bool is_scc_root = einfo.backlink == stack_index_[state];
                flags = einfo.flags;
                recursiveBacklink = einfo.backlink;
                queue_.pop_back();

                flags.complete = flags.complete && !terminated_;

                last_value_changed_ = false;
                last_policy_changed_ = false;

                if (backtrack_update_type_ == BacktrackingUpdateType::Single ||
                    (backtrack_update_type_ ==
                         BacktrackingUpdateType::OnDemand &&
                     (!flags.complete || !flags.all_solved))) {
                    ++statistics_.bw_updates;
                    auto updated = this->async_update(state, nullptr, nullptr);
                    last_value_changed_ = updated.first;
                    last_policy_changed_ = updated.second;
                    flags.complete = flags.complete && !last_policy_changed_;
                    flags.all_solved = flags.all_solved && !last_value_changed_;
                    terminated_ = terminated_ ||
                                  (terminate_exploration_ &&
                                   cutoff_inconsistent_ && last_value_changed_);
                }

                if (is_scc_root) {
                    const unsigned scc_size = stack_.size() - recursiveBacklink;
                    if (scc_size == 1) {
                        if (backtrack_update_type_ ==
                            BacktrackingUpdateType::UntilConvergence) {
                            auto res =
                                this->async_update(state, nullptr, nullptr);
                            last_value_changed_ = res.first;
                            last_policy_changed_ = res.second;
                            flags.complete = flags.complete && !res.second;
                            flags.all_solved = flags.all_solved && !res.first;
                            terminated_ =
                                terminated_ ||
                                (terminate_exploration_ &&
                                 cutoff_inconsistent_ && last_value_changed_);
                        }
                        backtrack_from_singleton(state, flags);
                    } else {
                        if (backtrack_update_type_ ==
                            BacktrackingUpdateType::UntilConvergence) {
                            auto res = value_iteration<true>(
                                stack_.begin() + recursiveBacklink,
                                stack_.end());
                            last_value_changed_ = res.first;
                            last_policy_changed_ = res.second;
                            flags.complete = flags.complete && !res.second;
                            flags.all_solved = flags.all_solved && !res.first;
                            terminated_ =
                                terminated_ ||
                                (terminate_exploration_ &&
                                 cutoff_inconsistent_ && last_value_changed_);
                        }
                        backtrack_from_non_singleton(
                            state,
                            flags,
                            stack_.begin() + recursiveBacklink);
                    }
                    recursiveBacklink = std::numeric_limits<int>::max();
                }
            }
        } while (!queue_.empty());

        assert(queue_.empty());
        assert(stack_.empty());
        stack_index_.clear();

        return flags.complete && flags.all_solved;
    }

    void backtrack_from_singleton(const StateID state, Flags& flags)
    {
        assert(stack_.back() == state);

        if (flags.complete && flags.all_solved) {
            if (mark_solved_) {
                this->get_state_info(state).set_solved();
            } else if (value_iteration_) {
                visited_states_.push_back(state);
            }
        }

        stack_index_[state] = STATE_CLOSED;
        stack_.pop_back();
        flags.is_trap = false;
    }

    void backtrack_from_non_singleton(
        const StateID state,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin)
    {
        assert(!flags.is_trap || !flags.complete || flags.all_solved);

        if (flags.complete && flags.all_solved) {
            if (flags.is_trap) {
                backtrack_trap(state, flags, scc_begin);
            } else {
                backtrack_solved(state, flags, scc_begin);
            }
        } else {
            backtrack_unsolved(state, flags, scc_begin);
        }
    }

    void backtrack_trap(
        const StateID state,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin)
    {
        assert(flags.dead);
        ++this->statistics_.traps;

        for (auto it = scc_begin; it != stack_.end(); ++it) {
            stack_index_[*it] = STATE_CLOSED;
        }

        statistics_.trap_timer.resume();
        quotient_->build_quotient(scc_begin, stack_.end(), state);
        this->get_state_info(state).set_policy(ActionID::undefined);
        stack_.erase(scc_begin, stack_.end());
        repush_trap(state, flags);
        statistics_.trap_timer.stop();
    }

    void backtrack_solved(
        const StateID,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin)
    {
        if (mark_solved_) {
            for (auto it = scc_begin; it != stack_.end(); ++it) {
                stack_index_[*it] = STATE_CLOSED;
                this->get_state_info(*it).set_solved();
            }
        } else if (value_iteration_) {
            for (auto it = scc_begin; it != stack_.end(); ++it) {
                stack_index_[*it] = STATE_CLOSED;
                visited_states_.push_back(*it);
            }
        } else {
            assert(false); // inconsistent parameters
        }

        flags.is_trap = false;
        stack_.erase(scc_begin, stack_.end());
    }

    void backtrack_unsolved(
        const StateID,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin)
    {
        for (auto it = scc_begin; it != stack_.end(); ++it) {
            stack_index_[*it] = STATE_CLOSED;
        }

        flags.is_trap = false;
        stack_.erase(scc_begin, stack_.end());
    }

    template <bool Convergence, typename StateIterator>
    std::pair<bool, bool>
    value_iteration(StateIterator begin, StateIterator end)
    {
        bool gvc = false;
        bool gpc = false;
        do {
            bool changed = false;
            for (auto it = begin; it != end; ++it) {
                auto updated = this->async_update(*it, nullptr, nullptr);
                changed = changed || updated.first;
                gvc = gvc || updated.first;
                gpc = gpc || updated.second;
                ++statistics_.bw_updates;
            }
            
            if constexpr (!Convergence) {
                break;
            } else {
                if (!changed) {
                    break;
                }
            }
        } while (true);
        return std::pair<bool, bool>(gvc, gpc);
    }

    QuotientSystem* quotient_;

    const bool forward_updates_;
    const bool expand_tip_states_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool mark_solved_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool reexpand_removed_traps_;

    engine_interfaces::OpenList<QAction>* open_list_;

    bool terminated_;

    int reexpansion_counter_;
    bool last_value_changed_;
    bool last_policy_changed_;

    std::deque<ExplorationInformation> queue_;
    std::vector<StateID> stack_;
    std::vector<StateID> visited_states_;
    storage::StateHashMap<int> stack_index_;

    Distribution<StateID> transition_;
    bool has_zero_reward_;

    internal::Statistics statistics_;
};

} // namespace trap_aware_dfhs
} // namespace engines
} // namespace probfd
