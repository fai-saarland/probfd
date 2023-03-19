#ifndef PROBFD_ENGINES_TRAP_AWARE_DFHS_H
#define PROBFD_ENGINES_TRAP_AWARE_DFHS_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"

#include "utils/timer.h"

#include <cassert>
#include <iterator>
#include <limits>
#include <ranges>
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

    utils::Timer trap_timer = utils::Timer(true);
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

template <typename State, typename Action, bool UseInterval>
class TADepthFirstHeuristicSearch;

template <typename State, typename Action, bool UseInterval>
class TADepthFirstHeuristicSearch<
    State,
    quotients::QuotientAction<Action>,
    UseInterval>
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

    using UpdateResult = typename HeuristicSearchBase::UpdateResult;

    using QAction = quotients::QuotientAction<Action>;
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using StateInfo = typename HeuristicSearchBase::StateInfo;

    struct Flags {
        /// was the exploration cut off?
        bool complete = true;
        /// were all reached outside-SCC states marked solved?
        bool all_solved = true;
        /// were all reached outside-SCC states marked dead end?
        bool dead = true;
        /// are there any outside-SCC states reachable, and do all transitions
        /// within the SCC generate 0-cost?
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

    static constexpr int STATE_UNSEEN = -1;
    static constexpr int STATE_CLOSED = -2;

    QuotientSystem* quotient_;

    const bool forward_updates_;
    const BacktrackingUpdateType backtrack_update_type_;
    const bool expand_tip_states_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool mark_solved_;
    const bool reexpand_removed_traps_;

    engine_interfaces::OpenList<QAction>* open_list_;

    bool terminated_ = false;

    std::deque<ExplorationInformation> queue_;
    std::vector<StateID> stack_;
    std::vector<StateID> visited_states_;
    storage::StateHashMap<int> stack_index_;

    Distribution<StateID> transition_;

    internal::Statistics statistics_;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADepthFirstHeuristicSearch(
        engine_interfaces::StateSpace<State, QAction>* state_space,
        engine_interfaces::CostFunction<State, QAction>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<QAction>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
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
              state_space,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison,
              stable_policy)
        , quotient_(quotient)
        , forward_updates_(forward_updates)
        , backtrack_update_type_(backtrack_update_type)
        , expand_tip_states_(expand_tip_states)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , terminate_exploration_(stop_exploration_inconsistent)
        , value_iteration_(value_iteration)
        , mark_solved_(mark_solved)
        , reexpand_removed_traps_(reexpand_removed_traps)
        , open_list_(open_list)
        , stack_index_(STATE_UNSEEN)
    {
    }

protected:
    Interval do_solve(const State& qstate) override
    {
        StateID state_id = this->get_state_id(qstate);
        if (value_iteration_) {
            dfhs_vi_driver(state_id);
        } else {
            dfhs_label_driver(state_id);
        }
        return this->lookup_dual_bounds(state_id);
    }

    void print_additional_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    void setup_custom_reports(const State&) override
    {
        statistics_.register_report(this->report_);
    }

private:
    void dfhs_vi_driver(const StateID state)
    {
        UpdateResult vi_res{true, true};
        do {
            const bool is_complete = policy_exploration(state);
            if (is_complete) {
                vi_res = value_iteration<false>(visited_states_);
            }
            visited_states_.clear();
            ++statistics_.iterations;
            this->print_progress();
        } while (vi_res.value_changed || vi_res.policy_changed);
    }

    void dfhs_label_driver(const StateID state)
    {
        bool is_complete = false;
        do {
            is_complete = policy_exploration(state) &&
                          this->get_state_info(state).is_solved();
            visited_states_.clear();
            ++statistics_.iterations;
            this->print_progress();
        } while (!is_complete);
    }

    void enqueue(StateID state, bool zero_cost)
    {
        queue_.emplace_back(state, stack_.size());
        stack_index_[state] = stack_.size();
        stack_.push_back(state);

        ExplorationInformation& info = queue_.back();
        info.successors.reserve(transition_.size());

        if (open_list_ == nullptr) {
            for (const StateID item : transition_.elements()) {
                if (item != state) {
                    info.successors.push_back(item);
                }
            }
        } else {
            const QAction a = this->get_policy(state);
            for (const auto& [item, probability] : transition_) {
                if (item != state) {
                    open_list_->push(state, a, probability, item);
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
        info.flags.is_trap = zero_cost;
    }

    bool push_state(StateID state, StateInfo& state_info, Flags& flags)
    {
        assert(!terminated_);
        assert(!state_info.is_solved() && !state_info.is_terminal());

        bool zero_cost;

        const bool tip = state_info.is_on_fringe();
        if (tip || forward_updates_) {
            ++statistics_.fw_updates;
            ActionID greedy_action;
            bool value_changed =
                this->async_update(state, &greedy_action, &transition_)
                    .value_changed;
            flags.all_solved = flags.all_solved && !value_changed;
            const bool cutoff = (!expand_tip_states_ && tip) ||
                                (cutoff_inconsistent_ && value_changed);
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
            zero_cost = this->get_action_cost(
                            state,
                            this->lookup_action(state, greedy_action)) == 0;
        } else {
            QAction action = this->get_policy(state);
            this->generate_successors(state, action, transition_);
            zero_cost = this->get_action_cost(state, action) == 0;
        }

        enqueue(state, zero_cost);
        return true;
    }

    bool push_state(StateID state, Flags& flags)
    {
        StateInfo& state_info = this->get_state_info(state);
        if (state_info.is_terminal() || state_info.is_solved()) {
            state_info.set_solved();
            flags.update(state_info);
            return false;
        }

        return push_state(state, state_info, flags);
    }

    bool repush_trap(StateID state, Flags& flags)
    {
        flags.clear();
        ++statistics_.fw_updates;

        ActionID greedy_action;
        bool value_changed =
            this->async_update(state, &greedy_action, &transition_)
                .value_changed;
        flags.all_solved = !value_changed;
        const bool cutoff = !reexpand_removed_traps_ ||
                            (cutoff_inconsistent_ && value_changed);
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
        const bool zero_cost =
            this->get_action_cost(
                state,
                this->lookup_action(state, greedy_action)) == 0;
        enqueue(state, zero_cost);
        return true;
    }

    bool policy_exploration(StateID start_state)
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
            const StateID state = einfo.state;

            einfo.backlink = std::min(recursiveBacklink, einfo.backlink);
            einfo.flags.update(flags);

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
                        flags.clear();
                        recursiveBacklink = std::numeric_limits<int>::max();
                        goto continue_exploration;
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

            flags = einfo.flags;
            recursiveBacklink = einfo.backlink;
            queue_.pop_back();

            flags.complete = flags.complete && !terminated_;

            if (backtrack_update_type_ == BacktrackingUpdateType::Single ||
                (backtrack_update_type_ == BacktrackingUpdateType::OnDemand &&
                 (!flags.complete || !flags.all_solved))) {
                ++statistics_.bw_updates;
                auto updated = this->async_update(state, nullptr, nullptr);
                flags.complete = flags.complete && !updated.policy_changed;
                flags.all_solved = flags.all_solved && !updated.value_changed;
                terminated_ = terminated_ ||
                              (terminate_exploration_ && cutoff_inconsistent_ &&
                               updated.value_changed);
            }

            // Is SCC root?
            if (einfo.backlink == stack_index_[state]) {
                const auto scc_begin = stack_.begin() + recursiveBacklink;
                const auto scc_end = stack_.end();
                const unsigned scc_size = std::distance(scc_begin, scc_end);

                if (scc_size == 1) {
                    if (backtrack_update_type_ ==
                        BacktrackingUpdateType::UntilConvergence) {
                        auto res = this->async_update(state, nullptr, nullptr);
                        flags.complete = flags.complete && !res.policy_changed;
                        flags.all_solved =
                            flags.all_solved && !res.value_changed;
                        terminated_ = terminated_ || (terminate_exploration_ &&
                                                      cutoff_inconsistent_ &&
                                                      res.value_changed);
                    }
                    backtrack_from_singleton(state, flags);
                } else {
                    if (backtrack_update_type_ ==
                        BacktrackingUpdateType::UntilConvergence) {
                        auto res = value_iteration<true>(
                            std::ranges::subrange(scc_begin, scc_end));
                        flags.complete = flags.complete && !res.policy_changed;
                        flags.all_solved =
                            flags.all_solved && !res.value_changed;
                        terminated_ = terminated_ || (terminate_exploration_ &&
                                                      cutoff_inconsistent_ &&
                                                      res.value_changed);
                    }
                    backtrack_from_non_singleton(
                        state,
                        flags,
                        scc_begin,
                        scc_end);
                }
                recursiveBacklink = std::numeric_limits<int>::max();
            }

        continue_exploration:;
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
        std::vector<StateID>::iterator scc_begin,
        std::vector<StateID>::iterator scc_end)
    {
        assert(!flags.is_trap || !flags.complete || flags.all_solved);

        if (flags.complete && flags.all_solved) {
            if (flags.is_trap) {
                backtrack_trap(state, flags, scc_begin, scc_end);
            } else {
                backtrack_solved(state, flags, scc_begin, scc_end);
            }
        } else {
            backtrack_unsolved(state, flags, scc_begin, scc_end);
        }
    }

    void backtrack_trap(
        const StateID state,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin,
        std::vector<StateID>::iterator scc_end)
    {
        assert(flags.dead);
        ++this->statistics_.traps;

        for (auto it = scc_begin; it != scc_end; ++it) {
            stack_index_[*it] = STATE_CLOSED;
        }

        utils::TimerScope scope(statistics_.trap_timer);

        // Get the greedy actions and collapse them
        std::vector<std::vector<Action>> collapsed_actions;
        for (auto it = scc_begin; it != scc_end; ++it) {
            collapsed_actions.emplace_back(std::initializer_list<Action>{
                quotient_->get_original_action(*it, this->get_policy(*it))});
        }

        quotient_->build_quotient(
            scc_begin,
            scc_end,
            state,
            collapsed_actions.begin());
        this->get_state_info(state).set_policy(ActionID::undefined);
        stack_.erase(scc_begin, scc_end);
        repush_trap(state, flags);
    }

    void backtrack_solved(
        const StateID,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin,
        std::vector<StateID>::iterator scc_end)
    {
        if (mark_solved_) {
            for (auto it = scc_begin; it != scc_end; ++it) {
                stack_index_[*it] = STATE_CLOSED;
                this->get_state_info(*it).set_solved();
            }
        } else if (value_iteration_) {
            for (auto it = scc_begin; it != scc_end; ++it) {
                stack_index_[*it] = STATE_CLOSED;
                visited_states_.push_back(*it);
            }
        } else {
            abort(); // inconsistent parameters
        }

        flags.is_trap = false;
        stack_.erase(scc_begin, scc_end);
    }

    void backtrack_unsolved(
        const StateID,
        Flags& flags,
        std::vector<StateID>::iterator scc_begin,
        std::vector<StateID>::iterator scc_end)
    {
        for (auto it = scc_begin; it != scc_end; ++it) {
            stack_index_[*it] = STATE_CLOSED;
        }

        flags.is_trap = false;
        stack_.erase(scc_begin, scc_end);
    }

    template <bool Convergence>
    UpdateResult value_iteration(const std::ranges::input_range auto& range)
    {
        UpdateResult result{false, false};

        for (;;) {
            bool changed = false;
            for (const StateID state : range) {
                auto updated = this->async_update(state, nullptr, nullptr);
                changed = changed || updated.value_changed;
                result.value_changed =
                    result.value_changed || updated.value_changed;
                result.policy_changed =
                    result.policy_changed || updated.policy_changed;
                ++statistics_.bw_updates;
            }

            if constexpr (!Convergence) {
                break;
            } else {
                if (!changed) {
                    break;
                }
            }
        }

        return result;
    }
};

template <typename State, typename Action, bool UseInterval>
class TADepthFirstHeuristicSearch : public MDPEngineInterface<State, Action> {
    TADepthFirstHeuristicSearch<
        State,
        quotients::QuotientAction<Action>,
        UseInterval>
        engine_;

    using QAction = quotients::QuotientAction<Action>;

public:
    /**
     * @brief Constructs a trap-aware DFHS solver object.
     */
    TADepthFirstHeuristicSearch(
        engine_interfaces::StateSpace<State, QAction>* state_space,
        engine_interfaces::CostFunction<State, QAction>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<QAction>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy,
        quotients::QuotientSystem<State, Action>* quotient,
        bool forward_updates,
        BacktrackingUpdateType backtrack_update_type,
        bool expand_tip_states,
        bool cutoff_inconsistent,
        bool stop_exploration_inconsistent,
        bool value_iteration,
        bool mark_solved,
        bool reexpand_removed_traps,
        engine_interfaces::OpenList<QAction>* open_list)
        : engine_(
              state_space,
              cost_function,
              value_init,
              policy_chooser,
              new_state_handler,
              report,
              interval_comparison,
              stable_policy,
              quotient,
              forward_updates,
              backtrack_update_type,
              expand_tip_states,
              cutoff_inconsistent,
              stop_exploration_inconsistent,
              value_iteration,
              mark_solved,
              reexpand_removed_traps,
              open_list)
    {
    }

    Interval solve(const State& state) override { return engine_.solve(state); }

    void print_statistics(std::ostream& out) const override
    {
        return engine_.print_statistics(out);
    }
};

} // namespace trap_aware_dfhs
} // namespace engines
} // namespace probfd

#endif