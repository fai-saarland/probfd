#ifndef PROBFD_ENGINES_TRAP_AWARE_DFHS_H
#define PROBFD_ENGINES_TRAP_AWARE_DFHS_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engines/heuristic_search_base.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/policies/map_policy.h"

#include "probfd/utils/guards.h"

#include "utils/countdown_timer.h"
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
    DISABLED,
    ON_DEMAND,
    SINGLE,
    CONVERGENCE,
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
    friend class TADepthFirstHeuristicSearch<State, Action, UseInterval>;

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
            , lowlink(stack_index)
        {
        }
        StateID state;
        std::vector<StateID> successors;
        Flags flags;
        int lowlink;
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
    std::vector<StackInfo> stack_;
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
        engine_interfaces::PolicyPicker<State, QAction>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
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
              interval_comparison)
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
    Interval do_solve(const State& qstate, double max_time) override
    {
        utils::CountdownTimer timer(max_time);

        StateID state_id = this->get_state_id(qstate);
        if (value_iteration_) {
            dfhs_vi_driver(state_id, timer);
        } else {
            dfhs_label_driver(state_id, timer);
        }
        return this->lookup_bounds(state_id);
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
    void dfhs_vi_driver(const StateID state, utils::CountdownTimer& timer)
    {
        UpdateResult vi_res{true, true};
        do {
            const bool is_complete = policy_exploration(state, timer);
            if (is_complete) {
                vi_res = value_iteration<false>(visited_states_, timer);
            }
            visited_states_.clear();
            ++statistics_.iterations;
            this->print_progress();
        } while (vi_res.value_changed || vi_res.policy_changed);
    }

    void dfhs_label_driver(const StateID state, utils::CountdownTimer& timer)
    {
        bool is_complete = false;
        do {
            is_complete = policy_exploration(state, timer) &&
                          this->get_state_info(state).is_solved();
            visited_states_.clear();
            ++statistics_.iterations;
            this->print_progress();
        } while (!is_complete);
    }

    void enqueue(StateID state, QAction& action, bool zero_cost)
    {
        queue_.emplace_back(state, stack_.size());
        stack_index_[state] = stack_.size();
        stack_.emplace_back(state, action);

        ExplorationInformation& info = queue_.back();
        info.successors.reserve(transition_.size());

        if (open_list_ == nullptr) {
            for (const StateID item : transition_.support()) {
                if (item != state) {
                    info.successors.push_back(item);
                }
            }
        } else {
            for (const auto& [item, probability] : transition_) {
                if (item != state) {
                    open_list_->push(state, action, probability, item);
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
            auto result = this->async_update(state, &transition_);
            flags.all_solved = flags.all_solved && !result.value_changed;
            const bool cutoff = (!expand_tip_states_ && tip) ||
                                (cutoff_inconsistent_ && result.value_changed);
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
            zero_cost =
                this->get_action_cost(state, *result.policy_action) == 0;
            enqueue(state, *result.policy_action, zero_cost);

        } else {
            QAction action = this->get_policy(state);
            this->generate_successors(state, action, transition_);
            zero_cost = this->get_action_cost(state, action) == 0;
            enqueue(state, action, zero_cost);
        }

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

        auto result = this->async_update(state, &transition_);
        flags.all_solved = !result.value_changed;
        const bool cutoff = !reexpand_removed_traps_ ||
                            (cutoff_inconsistent_ && result.value_changed);
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
            this->get_action_cost(state, *result.policy_action) == 0;
        enqueue(state, *result.policy_action, zero_cost);
        return true;
    }

    bool policy_exploration(StateID start_state, utils::CountdownTimer& timer)
    {
        using enum BacktrackingUpdateType;

        assert(visited_states_.empty());
        terminated_ = false;
        if (Flags flags; !push_state(start_state, flags)) {
            return flags.complete;
        }

        ExplorationInformation* einfo = &queue_.back();
        StateID state = einfo->state;

        for (;;) {
            do {
                timer.throw_if_expired();

                const StateID succ =
                    quotient_->translate_state_id(einfo->successors.back());

                int& succ_status = stack_index_[succ];
                if (succ_status == STATE_UNSEEN) {
                    // expand state (either not expanded before, or last
                    // value change was before pushing einfo->state)
                    StateInfo& succ_info = this->get_state_info(succ);
                    if (succ_info.is_terminal() || succ_info.is_solved()) {
                        succ_info.set_solved();
                        einfo->flags.update(succ_info);
                    } else if (push_state(succ, succ_info, einfo->flags)) {
                        einfo = &queue_.back();
                        state = einfo->state;
                        continue;
                    } else if (mark_solved_) {
                        einfo->flags.all_solved = false;
                    }
                    succ_status = STATE_CLOSED;
                } else if (succ_status == STATE_CLOSED) {
                    const StateInfo& succ_info = this->get_state_info(succ);
                    einfo->flags.update(succ_info);
                    if (mark_solved_) {
                        einfo->flags.all_solved =
                            einfo->flags.all_solved && succ_info.is_solved();
                    }
                } else {
                    // is on stack
                    einfo->lowlink = std::min(einfo->lowlink, succ_status);
                }

                einfo->successors.pop_back();
            } while (!terminated_ && !einfo->successors.empty());

            do {
                Flags flags = einfo->flags;
                const int last_lowlink = einfo->lowlink;
                queue_.pop_back();

                flags.complete = flags.complete && !terminated_;

                if (backtrack_update_type_ == SINGLE ||
                    (backtrack_update_type_ == ON_DEMAND &&
                     (!flags.complete || !flags.all_solved))) {
                    ++statistics_.bw_updates;
                    auto updated = this->async_update(state, nullptr);
                    flags.complete = flags.complete && !updated.policy_changed;
                    flags.all_solved =
                        flags.all_solved && !updated.value_changed;
                    terminated_ = terminated_ || (terminate_exploration_ &&
                                                  cutoff_inconsistent_ &&
                                                  updated.value_changed);
                }

                // Is SCC root?
                if (einfo->lowlink == stack_index_[state]) {
                    std::ranges::subrange scc(
                        stack_.begin() + last_lowlink,
                        stack_.end());

                    if (scc.size() == 1) {
                        if (backtrack_update_type_ == CONVERGENCE) {
                            auto res = this->async_update(state, nullptr);
                            flags.complete =
                                flags.complete && !res.policy_changed;
                            flags.all_solved =
                                flags.all_solved && !res.value_changed;
                            terminated_ =
                                terminated_ ||
                                (terminate_exploration_ &&
                                 cutoff_inconsistent_ && res.value_changed);
                        }
                        backtrack_from_singleton(state, flags);
                    } else {
                        if (backtrack_update_type_ == CONVERGENCE) {
                            auto res = value_iteration<true>(
                                scc |
                                    std::views::transform(&StackInfo::state_id),
                                timer);
                            flags.complete =
                                flags.complete && !res.policy_changed;
                            flags.all_solved =
                                flags.all_solved && !res.value_changed;
                            terminated_ =
                                terminated_ ||
                                (terminate_exploration_ &&
                                 cutoff_inconsistent_ && res.value_changed);
                        }
                        if (backtrack_from_non_singleton(state, flags, scc)) {
                            break; // re-expanded trap, continue exploring
                        }
                    }
                }

                if (queue_.empty()) {
                    assert(stack_.empty());
                    stack_index_.clear();
                    return flags.complete && flags.all_solved;
                }

                timer.throw_if_expired();

                einfo = &queue_.back();
                state = einfo->state;

                einfo->lowlink = std::min(last_lowlink, einfo->lowlink);
                einfo->flags.update(flags);

                einfo->successors.pop_back();
            } while (terminated_ || einfo->successors.empty());
        }
    }

    void backtrack_from_singleton(StateID state, Flags& flags)
    {
        assert(stack_.back().state_id == state);

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

    bool
    backtrack_from_non_singleton(const StateID state, Flags& flags, auto scc)
    {
        assert(!flags.is_trap || !flags.complete || flags.all_solved);

        if (flags.complete && flags.all_solved) {
            if (flags.is_trap) {
                return backtrack_trap(state, flags, scc);
            }

            backtrack_solved(state, flags, scc);
        } else {
            backtrack_unsolved(state, flags, scc);
        }

        return false;
    }

    bool backtrack_trap(const StateID state, Flags& flags, auto scc)
    {
        assert(flags.dead);
        ++this->statistics_.traps;

        for (const auto& entry : scc) {
            stack_index_[entry.state_id] = STATE_CLOSED;
        }

        TimerScope scope(statistics_.trap_timer);

        quotient_->build_quotient(scc, *scc.begin());
        this->get_state_info(state).set_policy(ActionID::undefined);
        stack_.erase(scc.begin(), scc.end());
        return repush_trap(state, flags);
    }

    void backtrack_solved(const StateID, Flags& flags, auto scc)
    {
        if (mark_solved_) {
            for (const auto& entry : scc) {
                stack_index_[entry.state_id] = STATE_CLOSED;
                this->get_state_info(entry.state_id).set_solved();
            }
        } else if (value_iteration_) {
            for (const auto& entry : scc) {
                stack_index_[entry.state_id] = STATE_CLOSED;
                visited_states_.push_back(entry.state_id);
            }
        } else {
            abort(); // inconsistent parameters
        }

        flags.is_trap = false;
        stack_.erase(scc.begin(), scc.end());
    }

    void backtrack_unsolved(const StateID, Flags& flags, auto scc)
    {
        for (const auto& entry : scc) {
            stack_index_[entry.state_id] = STATE_CLOSED;
        }

        flags.is_trap = false;
        stack_.erase(scc.begin(), scc.end());
    }

    template <bool Convergence>
    UpdateResult value_iteration(
        const std::ranges::input_range auto& range,
        utils::CountdownTimer& timer)
    {
        UpdateResult updated_all(false, false);
        bool value_changed;
        bool policy_changed;

        do {
            value_changed = false;
            policy_changed = false;

            for (const StateID id : range) {
                timer.throw_if_expired();

                const auto result = this->async_update(id, nullptr);
                value_changed = value_changed || result.value_changed;
                policy_changed = policy_changed || result.policy_changed;
            }

            updated_all.value_changed =
                updated_all.value_changed || value_changed;
            updated_all.policy_changed =
                updated_all.policy_changed || policy_changed;
        } while (value_changed && (Convergence || !policy_changed));

        return updated_all;
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
        engine_interfaces::PolicyPicker<State, QAction>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison,
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

    Interval solve(const State& state, double max_time) override
    {
        return engine_.solve(state, max_time);
    }

    std::unique_ptr<PartialPolicy<State, Action>>
    compute_policy(const State& state, double max_time) override
    {
        this->solve(state, max_time);

        auto* parent_state_space = engine_.quotient_->get_parent_state_space();

        /*
         * The quotient policy only specifies the optimal actions between
         * traps. We need to supplement the optimal actions within the
         * traps, i.e. the actions which point every other member state of
         * the trap towards that trap member state that owns the optimal
         * quotient action.
         *
         * We fully explore the quotient policy starting from the initial
         * state and compute the optimal 'inner' actions for each trap. To
         * this end, we first generate the sub-MDP of the trap. Afterwards,
         * we expand the trap graph backwards from the state that has the
         * optimal quotient action. For each encountered state, we select
         * the action with which it is encountered first as the policy
         * action.
         */

        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(parent_state_space));

        const StateID initial_state_id =
            parent_state_space->get_state_id(state);

        std::deque<StateID> queue({initial_state_id});
        std::set<StateID> visited({initial_state_id});

        do {
            const StateID quotient_id = queue.front();
            queue.pop_front();

            ActionID quotient_action_id = engine_.lookup_policy(quotient_id);

            // Terminal states have no policy decision.
            if (quotient_action_id == ActionID::undefined) {
                continue;
            }

            const QAction quotient_action =
                engine_.lookup_action(quotient_id, quotient_action_id);
            const Interval quotient_bound = engine_.lookup_bounds(quotient_id);

            const StateID exiting_id = quotient_action.state_id;

            policy->emplace_decision(
                exiting_id,
                quotient_action.action_id,
                quotient_bound);

            // Nothing else needs to be done if the trap has only one state.
            if (engine_.quotient_->quotient_size(quotient_id) != 1) {
                std::unordered_map<
                    StateID,
                    std::set<std::pair<StateID, ActionID>>>
                    parents;

                // Build the inverse graph
                std::vector<QAction> inner_actions;
                engine_.quotient_->get_pruned_ops(quotient_id, inner_actions);

                for (const QAction& qaction : inner_actions) {
                    StateID source_id = qaction.state_id;
                    ActionID action_id = qaction.action_id;

                    Distribution<StateID> successors;
                    parent_state_space->generate_action_transitions(
                        source_id,
                        parent_state_space->get_action(source_id, action_id),
                        successors);

                    for (const StateID succ_id : successors.support()) {
                        parents[succ_id].insert({source_id, action_id});
                    }
                }

                // Now traverse the inverse graph starting from the exiting
                // state
                std::deque<StateID> inverse_queue({exiting_id});
                std::set<StateID> inverse_visited({exiting_id});

                do {
                    const StateID next_id = inverse_queue.front();
                    inverse_queue.pop_front();

                    for (const auto& [pred_id, act_id] : parents[next_id]) {
                        if (inverse_visited.insert(pred_id).second) {
                            policy->emplace_decision(
                                pred_id,
                                act_id,
                                quotient_bound);
                            inverse_queue.push_back(pred_id);
                        }
                    }
                } while (!inverse_queue.empty());
            }

            // Push the successor traps.
            Distribution<StateID> successors;
            engine_.quotient_->generate_action_transitions(
                quotient_id,
                quotient_action,
                successors);

            for (const StateID succ_id : successors.support()) {
                if (visited.insert(succ_id).second) {
                    queue.push_back(succ_id);
                }
            }
        } while (!queue.empty());

        return policy;
    }

    value_t lookup_value(StateID state_id)
    {
        return engine_.lookup_value(state_id);
    }

    bool was_visited(StateID state_id) { return engine_.was_visited(state_id); }

    void print_statistics(std::ostream& out) const override
    {
        return engine_.print_statistics(out);
    }
};

} // namespace trap_aware_dfhs
} // namespace engines
} // namespace probfd

#endif