#ifndef PROBFD_ENGINES_HEURISTIC_SEARCH_BASE_H
#define PROBFD_ENGINES_HEURISTIC_SEARCH_BASE_H

#include "probfd/engines/heuristic_search_state_information.h"
#include "probfd/engines/utils.h"

#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/policies/map_policy.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/engine.h"
#include "probfd/progress_report.h"

#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <type_traits>
#include <vector>

namespace probfd {
namespace engines {

/// Namespace dedicated to the MDP h search base implementation
namespace heuristic_search {

namespace internal {

/**
 * @brief Base statistics for MDP h search.
 */
struct CoreStatistics {
    unsigned long long backups = 0;
    unsigned long long backed_up_states = 0;
    unsigned long long evaluated_states = 0;
    unsigned long long pruned_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long self_loop_states = 0;
    unsigned long long value_changes = 0;
    unsigned long long policy_updates = 0;
};

/**
 * @brief Extended statistics for MDP h search.
 */
struct Statistics : public CoreStatistics {
    unsigned state_info_bytes = 0;
    value_t initial_state_estimate = 0;
    bool initial_state_found_terminal = 0;

    value_t value = 0_vt;
    CoreStatistics before_last_update;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer update_time = utils::Timer(true);
    utils::Timer policy_selection_time = utils::Timer(true);
#endif

    /**
     * @brief Prints the statistics to the specified output stream.
     */
    void print(std::ostream& out) const
    {
        out << "  Stored " << state_info_bytes << " bytes per state"
            << std::endl;

        out << "  Initial state value estimation: " << initial_state_estimate
            << std::endl;
        out << "  Initial state value found terminal: "
            << initial_state_found_terminal << std::endl;

        out << "  Evaluated state(s): " << evaluated_states << std::endl;
        out << "  Pruned state(s): " << pruned_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Self-loop state(s): " << self_loop_states << std::endl;
        out << "  Backed up state(s): " << backed_up_states << std::endl;
        out << "  Number of backups: " << backups << std::endl;
        out << "  Number of value changes: " << value_changes << std::endl;
        out << "  Number of policy updates: " << policy_updates << std::endl;

        out << "  Evaluated state(s) until last value change: "
            << before_last_update.evaluated_states << std::endl;
        out << "  Pruned state(s) until last value change: "
            << before_last_update.pruned_states << std::endl;
        out << "  Goal state(s) until last value change: "
            << before_last_update.goal_states << std::endl;
        out << "  Terminal state(s) until last value change: "
            << before_last_update.terminal_states << std::endl;
        out << "  Self-loop state(s) until last value change: "
            << before_last_update.self_loop_states << std::endl;
        out << "  Backed up state(s) until last value change: "
            << before_last_update.backed_up_states << std::endl;
        out << "  Number of backups until last value change: "
            << before_last_update.backups << std::endl;
        out << "  Number of value changes until last value change: "
            << before_last_update.value_changes << std::endl;
        out << "  Number of policy updates until last value change: "
            << before_last_update.policy_updates << std::endl;

#if defined(EXPENSIVE_STATISTICS)
        out << "  Updating time: " << update_time << std::endl;
        out << "  Policy selection time: " << policy_selection_time
            << std::endl;
#endif
    }

    void jump() { before_last_update = *this; }
};

/**
 * @brief The common base class for MDP h search algorithms.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the undelying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class HeuristicSearchBase {
protected:
    using MDP = MDP<State, Action>;
    using Evaluator = Evaluator<State>;

    using PolicyPicker = engine_interfaces::PolicyPicker<State, Action>;
    using NewStateObserver = engine_interfaces::NewStateObserver<State>;

public:
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using EngineValueType = engines::EngineValueType<UseInterval>;

private:
    class StateInfos : public engine_interfaces::StateProperties {
        storage::PerStateStorage<StateInfo> state_infos_;

    public:
        StateInfo& operator[](StateID sid) { return state_infos_[sid]; }
        const StateInfo& operator[](StateID sid) const
        {
            return state_infos_[sid];
        }

        const StateFlags& lookup_state_flags(StateID state_id) override
        {
            return state_infos_[state_id];
        }

        value_t lookup_value(StateID state_id) override
        {
            return state_infos_[state_id].get_value();
        }

        Interval lookup_bounds(StateID state_id) override
        {
            return state_infos_[state_id].get_bounds();
        }
    };

    std::shared_ptr<PolicyPicker> policy_chooser_;
    std::shared_ptr<NewStateObserver> on_new_state_;

    StateInfos state_infos_;

    StateID initial_state_id_ = StateID::undefined;

    // Reused buffer
    std::vector<Transition<Action>> transitions_;

protected:
    Statistics statistics_;
    ProgressReport* report_;
    const bool interval_comparison_;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
        std::optional<Transition<Action>> greedy_transition = std::nullopt;
    };

public:
    HeuristicSearchBase(
        std::shared_ptr<PolicyPicker> policy_chooser,
        std::shared_ptr<NewStateObserver> new_state_handler,
        ProgressReport* report,
        bool interval_comparison)
        : policy_chooser_(policy_chooser)
        , on_new_state_(new_state_handler)
        , report_(report)
        , interval_comparison_(interval_comparison)
    {
        statistics_.state_info_bytes = sizeof(StateInfo);
    }

    value_t lookup_value(StateID state_id) const
    {
        return get_state_info(state_id).get_value();
    }

    Interval lookup_bounds(StateID state_id) const
    {
        return get_state_info(state_id).get_bounds();
    }

    /**
     * @brief Checks if the state \p state_id is terminal.
     */
    bool is_terminal(StateID state_id) const
    {
        return get_state_info(state_id).is_terminal();
    }

    /**
     * @brief Checks if the state represented by \p state_id is marked as a
     * dead-end.
     */
    bool is_marked_dead_end(StateID state_id) const
    {
        return get_state_info(state_id).is_dead_end();
    }

    /**
     * @brief Checks if the state represented by \p state_id has been visited
     * yet.
     */
    bool was_visited(StateID state_id) const
    {
        return get_state_info(state_id).is_value_initialized();
    }

    /**
     * @brief Clears the currently selected greedy action for the state
     * represented by \p state_id
     */
    void clear_policy(StateID state_id)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        get_state_info(state_id).clear_policy();
    }

    /**
     * @brief Calls notify_dead_end(StateInfo&) with the respective state info
     * object
     */
    bool notify_dead_end(StateID state_id)
    {
        return notify_dead_end(get_state_info(state_id));
    }

    /**
     * @brief Stores dead-end information in \p state_info. Returns true on
     * change.
     */
    bool notify_dead_end(StateInfo& state_info)
    {
        if (!state_info.is_dead_end()) {
            state_info.set_dead_end();
            state_info.value = EngineValueType(state_info.termination_cost);
            return true;
        }

        return false;
    }

    /**
     * @brief Calls notify_dead_end_ifnot_goal(StateInfo&) for the internal
     * state info object of \p state_id.
     */
    bool notify_dead_end_ifnot_goal(StateID state_id)
    {
        return notify_dead_end_ifnot_goal(get_state_info(state_id));
    }

    /**
     * @brief If no goal state flag was set, calls notify_dead_end(StateInfo&).
     *
     * Returns true if the goal flag was set.
     */
    bool notify_dead_end_ifnot_goal(StateInfo& state_info)
    {
        if (state_info.is_goal_state()) {
            return false;
        }

        notify_dead_end(state_info);

        return true;
    }

    std::optional<Action> get_greedy_action(StateID state_id)
        requires(StorePolicy)
    {
        return get_state_info(state_id).policy;
    }

    std::optional<Action>
    compute_greedy_action(MDP& mdp, Evaluator& h, StateID state_id)
        requires(!StorePolicy)
    {
        ClearGuard guard(transitions_);
        mdp.generate_all_transitions(state_id, transitions_);

        filter_greedy_transitions(
            mdp,
            h,
            state_id,
            lookup_initialize(mdp, h, state_id),
            transitions_);

        if (transitions_.empty()) return std::nullopt;

        const int index = this->policy_chooser_->pick_index(
            mdp,
            state_id,
            std::nullopt,
            transitions_,
            state_infos_);

        return transitions_[index].action;
    }

    /**
     * @brief Computes the Bellman update for a state and returns whether the
     * value changed.
     */
    bool bellman_update(MDP& mdp, Evaluator& h, StateID s)
    {
        return bellman_update(mdp, h, s, lookup_initialize(mdp, h, s));
    }

    /**
     * @brief Computes the Bellman update for a state and outputs all greedy
     * transitions, and returns whether the value changed.
     */
    bool bellman_update(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        std::vector<Transition<Action>>& greedy)
    {
        return bellman_update(
            mdp,
            h,
            state_id,
            lookup_initialize(mdp, h, state_id),
            greedy);
    }

    /**
     * @brief Computes the Bellman update for a state, recomputes the greedy
     * action for it, and outputs status changes and the new greedy transition.
     */
    UpdateResult bellman_policy_update(MDP& mdp, Evaluator& h, StateID state_id)
        requires(StorePolicy)
    {
        StateInfo& state_info = lookup_initialize(mdp, h, state_id);

        ClearGuard guard(transitions_);

        const bool value_change =
            bellman_update(mdp, h, state_id, state_info, transitions_);

        if (transitions_.empty()) {
            state_info.clear_policy();
            return UpdateResult{value_change, false};
        }

#if defined(EXPENSIVE_STATISTICS)
        TimerScope scoped(statistics_.policy_selection_time);
#endif

        ++statistics_.policy_updates;

        const int index = this->policy_chooser_->pick_index(
            mdp,
            state_id,
            state_info.get_policy(),
            transitions_,
            state_infos_);
        assert(utils::in_bounds(index, transitions_));

        auto& transition = transitions_[index];

        const bool policy_change = state_info.update_policy(transition.action);

        return UpdateResult{value_change, policy_change, std::move(transition)};
    }

protected:
    void initialize_report(MDP& mdp, Evaluator& h, param_type<State> state)
    {
        initial_state_id_ = mdp.get_state_id(state);

        StateInfo& info = get_state_info(initial_state_id_);

        if (!initialize_if_needed(mdp, h, initial_state_id_, info)) {
            return;
        }

        if constexpr (UseInterval) {
            report_->register_bound("v", [&info]() { return info.value; });
        } else {
            report_->register_bound("v", [&info]() {
                return Interval(info.value, INFINITE_VALUE);
            });
        }

        statistics_.value = info.get_value();
        statistics_.before_last_update = statistics_;
        statistics_.initial_state_estimate = info.get_value();
        statistics_.initial_state_found_terminal = info.is_terminal();
    }

    void print_statistics(std::ostream& out) const { statistics_.print(out); }

    /**
     * @brief Advances the progress report.
     */
    void print_progress() { this->report_->print(); }

    bool check_interval_comparison() const { return interval_comparison_; }

    /**
     * @brief Get the state info object of a state.
     */
    StateInfo& get_state_info(StateID id) { return state_infos_[id]; }

    /**
     * @brief Get the state info object of a state.
     */
    const StateInfo& get_state_info(StateID id) const
    {
        return state_infos_[id];
    }

    /**
     * @brief Get the state info object of a state, if needed.
     *
     * This method is used as a selection mechanism to obtain the correct
     * state information object for a state. Algorithms like LRTDP may or
     * may not store their algorithm specific state information separately
     * from the base state information stored in this class. This method
     * checks if the provided state info object is the required base state
     * information object by checking for type equality and returns it if that
     * is the case. Otherwise, the base state information object for this state
     * is retrieved and returned.
     */
    template <typename AlgStateInfo>
    StateInfo& get_state_info(StateID id, AlgStateInfo& info)
    {
        if constexpr (std::is_same_v<AlgStateInfo, StateInfo>) {
            return info;
        } else {
            return get_state_info(id);
        }
    }

    /**
     * @brief Get the state info object of a state, if needed.
     *
     * This method is used as a selection mechanism to obtain the correct
     * state information object for a state. Algorithms like LRTDP may or
     * may not store their algorithm specific state information separately
     * from the base state information stored in this class. This method
     * checks if the provided state info object is the required base state
     * information object by checking for type equality and returns it if that
     * is the case. Otherwise, the base state information object for this state
     * is retrieved and returned.
     */
    template <typename AlgStateInfo>
    const StateInfo& get_state_info(StateID id, const AlgStateInfo& info) const
    {
        if constexpr (std::is_same_v<AlgStateInfo, StateInfo>) {
            return info;
        } else {
            return get_state_info(id);
        }
    }

    StateID sample_state(
        engine_interfaces::SuccessorSampler<Action>& sampler,
        StateID source,
        const Distribution<StateID>& transition)
    {
        return sampler.sample(
            source,
            *this->get_greedy_action(source),
            transition,
            state_infos_);
    }

private:
    bool update(StateInfo& state_info, StateID state_id, EngineValueType other)
        requires(UseInterval)
    {
        const bool b =
            engines::update(state_info.value, other, interval_comparison_);
        if (b) state_value_changed(state_id);
        return b;
    }

    bool update(StateInfo& state_info, StateID state_id, EngineValueType other)
        requires(!UseInterval)
    {
        const bool b = engines::update(state_info.value, other);
        if (b) state_value_changed(state_id);
        return b;
    }

    void state_value_changed(StateID state_id)
    {
        ++statistics_.value_changes;
        if (state_id == initial_state_id_) {
            statistics_.jump();
        }
    }

    StateInfo& lookup_initialize(MDP& mdp, Evaluator& h, StateID state_id)
    {
        StateInfo& state_info = get_state_info(state_id);
        initialize_if_needed(mdp, h, state_id, state_info);
        return state_info;
    }

    bool initialize_if_needed(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info)
    {
        if (state_info.is_value_initialized()) return false;

        statistics_.evaluated_states++;

        State state = mdp.get_state(state_id);
        TerminationInfo term = mdp.get_termination_info(state);
        const value_t t_cost = term.get_cost();

        state_info.termination_cost = t_cost;
        if (term.is_goal_state()) {
            state_info.set_goal();
            state_info.value = EngineValueType(t_cost);
            statistics_.goal_states++;
            if (on_new_state_) on_new_state_->notify_goal(state);
            return true;
        }

        EvaluationResult estimate = h.evaluate(state);
        if (estimate.is_unsolvable()) {
            statistics_.pruned_states++;
            notify_dead_end(state_info);
            if (on_new_state_) on_new_state_->notify_dead(state);
        } else {
            state_info.set_on_fringe();

            if constexpr (UseInterval) {
                state_info.value.lower = estimate.get_estimate();
            } else {
                state_info.value = estimate.get_estimate();
            }

            if (on_new_state_) on_new_state_->notify_state(state);
        }

        return true;
    }

    std::optional<EngineValueType> normalized_qvalue(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        const Transition<Action>& transition)
    {
        EngineValueType t_value(mdp.get_action_cost(transition.action));
        value_t non_self_loop = 1_vt;
        bool loop = true;

        for (const auto& [succ_id, prob] : transition.successor_dist) {
            if (succ_id == state_id) {
                non_self_loop -= prob;
                continue;
            }

            const auto& succ_info = lookup_initialize(mdp, h, succ_id);
            t_value += prob * succ_info.value;
            loop = false;
        }

        if (loop) return std::nullopt;

        if (non_self_loop < 1_vt) {
            t_value *= (1 / non_self_loop);
        }

        return t_value;
    }

    EngineValueType filter_greedy_transitions(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info,
        std::vector<Transition<Action>>& transitions)
    {
        using std::ranges::remove_if;

        // First compute the (self-loop normalized) Q values for the lower
        // bound, the minimum Q value, and remove self-loop transitions in the
        // process
        EngineValueType best_value(state_info.termination_cost);

        std::vector<value_t> lower_bound_qvalues;
        lower_bound_qvalues.reserve(transitions.size());

        std::erase_if(transitions, [&, state_id](const auto& transition) {
            if (auto Q = normalized_qvalue(mdp, h, state_id, transition)) {
                set_min(best_value, *Q);
                lower_bound_qvalues.push_back(as_lower_bound(*Q));
                return false;
            }

            return true;
        });

        // Now filter non-epsilon-greedy transitions
        if (!transitions.empty()) {
            const value_t best = as_lower_bound(best_value);
            auto view = std::views::zip(transitions, lower_bound_qvalues);
            auto [it, end] = remove_if(view, approx_neq_to(best), project<1>);

            const size_t offset = std::distance(view.begin(), it);
            transitions.erase(transitions.begin() + offset, transitions.end());
        }

        return best_value;
    }

    bool bellman_update(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info)
    {
#if defined(EXPENSIVE_STATISTICS)
        TimerScope scoped_upd_timer(statistics_.update_time);
#endif
        statistics_.backups++;

        if (state_info.is_terminal()) {
            return false;
        }

        if (state_info.is_on_fringe()) {
            ++statistics_.backed_up_states;
            state_info.removed_from_fringe();
        }

        ClearGuard guard(transitions_);
        mdp.generate_all_transitions(state_id, transitions_);

        if (transitions_.empty()) {
            statistics_.terminal_states++;
            const bool result = notify_dead_end(state_info);
            if (result) state_value_changed(state_id);
            return result;
        }

        EngineValueType best_value(state_info.termination_cost);

        bool has_only_self_loops = true;
        for (auto& transition : transitions_) {
            if (auto Q = normalized_qvalue(mdp, h, state_id, transition)) {
                set_min(best_value, *Q);
                has_only_self_loops = false;
            }
        }

        if (has_only_self_loops) {
            statistics_.self_loop_states++;
            return notify_dead_end(state_info);
        }

        return this->update(state_info, state_id, best_value);
    }

    bool bellman_update(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info,
        std::vector<Transition<Action>>& greedy)
    {
#if defined(EXPENSIVE_STATISTICS)
        TimerScope scoped_upd_timer(statistics_.update_time);
#endif
        statistics_.backups++;

        if (state_info.is_terminal()) {
            return false;
        }

        if (state_info.is_on_fringe()) {
            ++statistics_.backed_up_states;
            state_info.removed_from_fringe();
        }

        mdp.generate_all_transitions(state_id, greedy);

        if (greedy.empty()) {
            statistics_.terminal_states++;
            const bool result = notify_dead_end(state_info);
            if (result) state_value_changed(state_id);
            return result;
        }

        EngineValueType best_value =
            filter_greedy_transitions(mdp, h, state_id, state_info, greedy);

        if (greedy.empty()) {
            statistics_.self_loop_states++;
            return notify_dead_end(state_info);
        }

        return this->update(state_info, state_id, best_value);
    }
};

/**
 * @brief Extends HeuristicSearchBase with default implementations for
 * MDPEngine.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the undelying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class HeuristicSearchEngine
    : public MDPEngine<State, Action>
    , public HeuristicSearchBase<State, Action, StateInfoT> {
    using EngineBase = typename HeuristicSearchEngine::MDPEngine;
    using HSBase = typename HeuristicSearchEngine::HeuristicSearchBase;

protected:
    using PartialPolicy = typename EngineBase::PartialPolicy;

    using MDP = typename EngineBase::MDP;
    using Evaluator = typename EngineBase::Evaluator;

    using StateInfo = typename HSBase::StateInfo;
    using PolicyPicker = typename HSBase::PolicyPicker;
    using NewStateObserver = typename HSBase::NewStateObserver;

    using MapPolicy = policies::MapPolicy<State, Action>;

public:
    // Inherited constructor
    using HSBase::HSBase;

    Interval solve(
        MDP& mdp,
        Evaluator& h,
        param_type<State> state,
        double max_time =
            std::numeric_limits<double>::infinity()) final override
    {
        HSBase::initialize_report(mdp, h, state);
        this->setup_custom_reports(state);
        return this->do_solve(mdp, h, state, max_time);
    }

    std::unique_ptr<PartialPolicy> compute_policy(
        MDP& mdp,
        Evaluator& h,
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) override
    {
        this->solve(mdp, h, state, max_time);

        /*
         * Expand some greedy policy graph, starting from the initial state.
         * Collect optimal actions along the way.
         */

        std::unique_ptr<MapPolicy> policy(new MapPolicy(&mdp));

        const StateID initial_state_id = mdp.get_state_id(state);

        std::deque<StateID> queue;
        std::set<StateID> visited;
        queue.push_back(initial_state_id);
        visited.insert(initial_state_id);

        do {
            const StateID state_id = queue.front();
            queue.pop_front();

            std::optional<Action> action;

            if constexpr (HSBase::StorePolicy) {
                action = this->get_greedy_action(state_id);
            } else {
                action = this->compute_greedy_action(mdp, h, state_id);
            }

            // Terminal states have no policy decision.
            if (!action) {
                continue;
            }

            const Interval bound = this->lookup_bounds(state_id);

            policy->emplace_decision(state_id, *action, bound);

            // Push the successor traps.
            Distribution<StateID> successors;
            mdp.generate_action_transitions(state_id, *action, successors);

            for (const StateID succ_id : successors.support()) {
                if (visited.insert(succ_id).second) {
                    queue.push_back(succ_id);
                }
            }
        } while (!queue.empty());

        return policy;
    }

    void print_statistics(std::ostream& out) const final override
    {
        HSBase::print_statistics(out);
        this->print_additional_statistics(out);
    }

    /**
     * @brief Sets up internal custom reports of a state in an implementation.
     */
    virtual void setup_custom_reports(param_type<State>) {}
    /**
     * @brief Resets the h search engine to a clean state.
     *
     * This method is needed by the FRET wrapper engine to restart the
     * h search after traps have been collapsed.
     */
    virtual void reset_search_state() {}

    /**
     * @brief Solves for the optimal state value of the input state.
     *
     * Called internally after initializing the progress report.
     */
    virtual Interval do_solve(
        MDP& mdp,
        Evaluator& h,
        param_type<State> state,
        double max_time) = 0;

    /**
     * @brief Prints additional statistics to the output stream.
     *
     * Called internally after printing the base h search statistics.
     */
    virtual void print_additional_statistics(std::ostream& out) const = 0;
};

} // namespace internal

template <typename T>
struct NoAdditionalStateData : public T {};

template <
    typename State,
    typename Action,
    bool UseInterval = false,
    bool StorePolicy = false,
    template <typename> class StateInfo = NoAdditionalStateData>
using HeuristicSearchBase = internal::HeuristicSearchBase<
    State,
    Action,
    StateInfo<PerStateBaseInformation<Action, StorePolicy, UseInterval>>>;

template <
    typename State,
    typename Action,
    bool UseInterval = false,
    bool StorePolicy = false,
    template <typename> class StateInfo = NoAdditionalStateData>
using HeuristicSearchEngine = internal::HeuristicSearchEngine<
    State,
    Action,
    StateInfo<PerStateBaseInformation<Action, StorePolicy, UseInterval>>>;

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_BASE_H__