#ifndef PROBFD_ENGINES_HEURISTIC_SEARCH_BASE_H
#define PROBFD_ENGINES_HEURISTIC_SEARCH_BASE_H

#include "probfd/engines/engine.h"
#include "probfd/engines/heuristic_search_state_information.h"
#include "probfd/engines/utils.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"
#include "probfd/engine_interfaces/new_state_observer.h"
#include "probfd/engine_interfaces/policy_picker.h"

#include "probfd/policies/map_policy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/progress_report.h"

#include "utils/collections.h"
#include "utils/system.h"

#if defined(EXPENSIVE_STATISTICS)
#include "utils/timer.h"
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

/// Namespace dedicated to the MDP heuristic search base implementation
namespace heuristic_search {

namespace internal {

/**
 * @brief Base statistics for MDP heuristic search.
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
 * @brief Extended statistics for MDP heuristic search.
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
 * @brief The common base class for MDP heuristic search algorithms.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the undelying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class HeuristicSearchBase
    : public MDPEngine<State, Action>
    , public engine_interfaces::HeuristicSearchInterface<Action> {
public:
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using EngineValueType = engines::EngineValueType<UseInterval>;

private:
    engine_interfaces::Evaluator<State>* value_initializer_;
    engine_interfaces::PolicyPicker<State, Action>* policy_chooser_;
    engine_interfaces::NewStateObserver<State>* on_new_state_;

    storage::PerStateStorage<StateInfo> state_infos_;

    Statistics statistics_;

    StateID initial_state_id_ = StateID::undefined;

protected:
    ProgressReport* report_;
    const bool interval_comparison_;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
        std::optional<Action> policy_action = std::nullopt;
    };

public:
    HeuristicSearchBase(
        engine_interfaces::StateSpace<State, Action>* state_space,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* value_init,
        engine_interfaces::PolicyPicker<State, Action>* policy_chooser,
        engine_interfaces::NewStateObserver<State>* new_state_handler,
        ProgressReport* report,
        bool interval_comparison)
        : MDPEngine<State, Action>(state_space, cost_function)
        , value_initializer_(value_init)
        , policy_chooser_(policy_chooser)
        , on_new_state_(new_state_handler)
        , report_(report)
        , interval_comparison_(interval_comparison)
    {
        statistics_.state_info_bytes = sizeof(StateInfo);
    }

    virtual ~HeuristicSearchBase() = default;

    Interval solve(
        param_type<State> state,
        double max_time =
            std::numeric_limits<double>::infinity()) final override
    {
        this->initialize_report(state);
        return this->do_solve(state, max_time);
    }

    std::unique_ptr<PartialPolicy<State, Action>> compute_policy(
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) override
    {
        this->solve(state, max_time);

        /*
         * Expand some greedy policy graph, starting from the initial state.
         * Collect optimal actions along the way.
         */

        std::unique_ptr<policies::MapPolicy<State, Action>> policy(
            new policies::MapPolicy<State, Action>(this->get_state_space()));

        const StateID initial_state_id = this->get_state_id(state);

        std::deque<StateID> queue;
        std::set<StateID> visited;
        queue.push_back(initial_state_id);
        visited.insert(initial_state_id);

        do {
            const StateID state_id = queue.front();
            queue.pop_front();

            std::optional action = this->lookup_policy(state_id);

            // Terminal states have no policy decision.
            if (!action) {
                continue;
            }

            const Interval bound = this->lookup_bounds(state_id);

            policy->emplace_decision(state_id, *action, bound);

            // Push the successor traps.
            Distribution<StateID> successors;
            this->generate_successors(state_id, *action, successors);

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
        statistics_.print(out);
        this->print_additional_statistics(out);
    }

    const StateFlags& lookup_state_flags(StateID state_id) override
    {
        return state_infos_[state_id];
    }

    value_t lookup_value(StateID state_id) override
    {
        if constexpr (UseInterval) {
            return state_infos_[state_id].value.lower;
        } else {
            return state_infos_[state_id].value;
        }
    }

    Interval lookup_bounds(StateID state_id) override
    {
        if constexpr (UseInterval) {
            return state_infos_[state_id].value;
        } else {
            return Interval(state_infos_[state_id].value, INFINITE_VALUE);
        }
    }

    std::optional<Action> lookup_policy(StateID state_id) override
    {
        if constexpr (!StorePolicy) {
            std::vector<Action> opt_aops;
            std::vector<Distribution<StateID>> opt_transitions;
            compute_optimal_transitions(
                state_id,
                lookup_initialize(state_id),
                opt_aops,
                opt_transitions);

            if (opt_aops.empty()) return std::nullopt;

            return opt_aops[this->policy_chooser_->pick_index(
                *this->get_state_space(),
                state_id,
                std::nullopt,
                opt_aops,
                opt_transitions,
                *this)];
        } else {
            return state_infos_[state_id].policy;
        }
    }

    /**
     * @brief Resets the heuristic search engine to a clean state.
     *
     * This method is needed by the FRET wrapper engine to restart the heuristic
     * search after traps have been collapsed.
     */
    virtual void reset_search_state() {}

    /**
     * @brief Checks if the state \p state_id is terminal.
     */
    bool is_terminal(StateID state_id) const
    {
        return state_infos_[state_id].is_terminal();
    }

    /**
     * @brief Checks if the state represented by \p state_id is marked as a
     * dead-end.
     */
    bool is_marked_dead_end(StateID state) const
    {
        return state_infos_[state].is_dead_end();
    }

    /**
     * @brief Checks if the state represented by \p state_id has been visited
     * yet.
     */
    bool was_visited(StateID state) const {
        return state_infos_[state].is_value_initialized();
    }

    /**
     * @brief Clears the currently selected greedy action for the state
     * represented by \p state_id
     */
    void clear_policy(StateID state_id)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        state_infos_[state_id].clear_policy();
    }

    /**
     * @brief Gets the currently selected greedy action for the state
     * represented by \p state_id
     */
    Action get_policy(StateID state_id)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        std::optional a = state_infos_[state_id].get_policy();
        assert(a.has_value());
        return *a;
    }

    /**
     * @brief Generates the successor distribution referring to the application
     * of the current greedy action in a state
     *
     * @param[in] state - The id of the source state
     * @param[out] result - The returned successor distribution when applying
     * the current greedy action in the state represented by \p state
     */
    bool apply_policy(StateID state_id, Distribution<StateID>& result)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        std::optional a = state_infos_[state_id].get_policy();

        if (!a) {
            return async_update(state_id, &result).value_changed;
        }

        this->generate_successors(state_id, *a, result);
        return false;
    }

    /**
     * @brief Calls notify_dead_end(StateID, StateInfo&) with the
     * respective state info object
     */
    bool notify_dead_end(StateID state_id)
    {
        return notify_dead_end(state_infos_[state_id]);
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
     * @brief Calls notify_dead_end_ifnot_goal(StateID, StateInfo&) for
     * the internal state info object of \p state_id.
     */
    bool notify_dead_end_ifnot_goal(StateID state_id)
    {
        return notify_dead_end_ifnot_goal(state_infos_[state_id]);
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

    /**
     * @brief Computes the value update for a state and returns whether the
     * value changed.
     *
     * If the policy is stored, the greedy action for s is also updated using
     * the internal policy tiebreaking settings.
     */
    bool async_update(StateID s)
    {
        if constexpr (!StorePolicy) {
            return compute_value_update(s, lookup_initialize(s));
        } else {
            return compute_value_policy_update(s, nullptr).value_changed;
        }
    }

    /**
     * @brief Computes the value and policy update for a state and optionally
     * outputs the new greedy transition.
     *
     * @note Output parameters may be nullptr. Only applicable if the policy is
     * stored.
     *
     * @param[in] s - The state for the value update
     * @param[out] policy_transition - Return address for the new greedy
     * transition.
     */
    UpdateResult
    async_update(StateID s, Distribution<StateID>* policy_transition)
    {
        return compute_value_policy_update(s, policy_transition);
    }

    bool compute_value_update_and_optimal_transitions(
        StateID state_id,
        std::vector<Action>& opt_aops,
        std::vector<Distribution<StateID>>& opt_transitions)
    {
        return compute_value_update_and_optimal_transitions(
            state_id,
            lookup_initialize(state_id),
            opt_aops,
            opt_transitions);
    }

protected:
    /**
     * @brief Solves for the optimal state value of the input state.
     *
     * Called internally after initializing the progress report.
     */
    virtual Interval do_solve(param_type<State> state, double max_time) = 0;

    /**
     * @brief Prints additional statistics to the output stream.
     *
     * Called internally after printing the base heuristic search statistics.
     */
    virtual void print_additional_statistics(std::ostream& out) const = 0;

    /**
     * @brief Sets up internal custom reports of a state in an implementation.
     */
    virtual void setup_custom_reports(param_type<State>) {}

    /**
     * @brief Advances the progress report.
     */
    void print_progress() { this->report_->print(); }

    /**
     * @brief Get the state info storage.
     */
    storage::PerStateStorage<StateInfo>& get_state_info_store()
    {
        return state_infos_;
    }

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
    StateInfo& get_state_info(const StateID id, AlgStateInfo& info)
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
    const StateInfo&
    get_state_info(const StateID id, const AlgStateInfo& info) const
    {
        if constexpr (std::is_same_v<AlgStateInfo, StateInfo>) {
            return info;
        } else {
            return get_state_info(id);
        }
    }

    /**
     * @brief Checks if the value bounds of the state are epsilon-close.
     *
     * @return False if interval comparison is not used.
     * Otherwise returns true iff the value bounds are not epsilon-close.
     */
    template <typename Info>
    bool do_bounds_disagree(StateID state_id, const Info& info)
    {
        if (!interval_comparison_) return false;

        if constexpr (std::is_same_v<Info, StateInfo>) {
            return !info.value.bounds_approximately_equal();
        } else {
            return !state_infos_[state_id].value.bounds_approximately_equal();
        }
    }

    /**
     * @brief Dumps the search space as a graph.
     *
     * State names are printed as specified by the state-to-string lambda
     * function object.
     *
     * @tparam StateToString - Type of the state-to-string function object.
     * @param file_name - The output file name.
     * @param sstr -The state-to-string lambda function.
     */
    void dump_search_space(
        const std::string& file_name,
        const std::function<std::string(param_type<State>)> sstr =
            [](param_type<State>) { return ""; })
    {
        struct ExpansionCondition : public engine_interfaces::Evaluator<State> {
            explicit ExpansionCondition(
                const MDPEngine<State, Action>* hs,
                storage::PerStateStorage<StateInfo>* infos)
                : hs_(hs)
                , infos_(infos)
            {
            }

            EvaluationResult operator()(param_type<State> state) const override
            {
                const StateID sid = hs_->get_state_id(state);
                const StateInfo& info = infos_->operator[](sid);
                return EvaluationResult(info.is_on_fringe(), 0);
            }

            const MDPEngine<State, Action>* hs_;
            storage::PerStateStorage<StateInfo>* infos_;
        };

        ExpansionCondition prune(this, &state_infos_);

        std::ofstream out(file_name);

        graphviz::dump_state_space_dot_graph<State, Action>(
            out,
            this->lookup_state(initial_state_id_),
            this->get_state_id_map(),
            this->get_state_cost_function(),
            this->get_applicable_actions_generator(),
            this->get_transition_generator(),
            sstr,
            [](const Action&) { return ""; },
            &prune,
            false);
    }

private:
    void initialize_report(param_type<State> state)
    {
        initial_state_id_ = this->get_state_id(state);

        if (state_infos_[initial_state_id_].is_value_initialized()) {
            return;
        }

        const StateInfo& info = lookup_initialize(initial_state_id_);

        if constexpr (UseInterval) {
            report_->register_bound("v", [&info]() { return info.value; });
        } else {
            report_->register_bound("v", [&info]() {
                return Interval(info.value, INFINITE_VALUE);
            });
        }

        statistics_.value = as_lower_bound(info.value);
        statistics_.before_last_update = statistics_;
        statistics_.initial_state_estimate = as_lower_bound(info.value);
        statistics_.initial_state_found_terminal = info.is_terminal();

        setup_custom_reports(state);
    }

    bool update(StateInfo& state_info, const EngineValueType& other)
    {
        if constexpr (UseInterval) {
            return engines::update(
                state_info.value,
                other,
                interval_comparison_);
        } else {
            return engines::update(state_info.value, other);
        }
    }

    StateInfo& lookup_initialize(StateID state_id)
    {
        StateInfo& state_info = state_infos_[state_id];

        if (!state_info.is_value_initialized()) {
            statistics_.evaluated_states++;

            State state = this->lookup_state(state_id);
            TerminationInfo term = this->get_termination_info(state);
            const value_t t_cost = term.get_cost();

            state_info.termination_cost = t_cost;
            if (term.is_goal_state()) {
                state_info.set_goal();
                state_info.value = EngineValueType(t_cost);
                statistics_.goal_states++;
                if (on_new_state_) on_new_state_->notify_goal(state);
                return state_info;
            }

            EvaluationResult estimate = value_initializer_->evaluate(state);
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
        }

        return state_info;
    }

    bool compute_value_update(StateID state_id, StateInfo& state_info)
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

        std::vector<Action> aops;
        std::vector<Distribution<StateID>> transitions;
        this->generate_all_successors(state_id, aops, transitions);

        assert(aops.size() == transitions.size());

        if (aops.empty()) {
            statistics_.terminal_states++;
            const bool result = notify_dead_end(state_info);
            if (result) {
                ++statistics_.value_changes;
                if (state_id == initial_state_id_) {
                    statistics_.jump();
                }
            }
            return result;
        }

        EngineValueType new_value(state_info.termination_cost);

        bool has_only_self_loops = true;
        for (unsigned i = 0; i < aops.size(); ++i) {
            Action& op = aops[i];
            Distribution<StateID>& transition = transitions[i];

            EngineValueType t_value(this->get_action_cost(state_id, op));
            value_t self_loop = 0_vt;
            bool non_loop = false;

            for (const auto& [succ_id, prob] : transition) {
                if (succ_id == state_id) {
                    self_loop += prob;
                } else {
                    const StateInfo& succ_info = lookup_initialize(succ_id);
                    t_value += prob * succ_info.value;
                    non_loop = true;
                }
            }

            if (non_loop) {
                if (self_loop > 0_vt) {
                    t_value *= 1_vt / (1_vt - self_loop);
                }

                set_min(new_value, t_value);
                has_only_self_loops = false;
            }
        }

        if (has_only_self_loops) {
            statistics_.self_loop_states++;
            return notify_dead_end(state_info);
        }

        if (this->update(state_info, new_value)) {
            ++statistics_.value_changes;
            if (state_id == initial_state_id_) {
                statistics_.jump();
            }
            return true;
        }

        return false;
    }

    EngineValueType compute_non_loop_transitions_and_values(
        StateID state_id,
        StateInfo& state_info,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& transitions,
        std::vector<EngineValueType>& values)
    {
        this->generate_all_successors(state_id, aops, transitions);

        assert(aops.size() == transitions.size());

        values.reserve(aops.size());

        EngineValueType best_value(state_info.termination_cost);

        unsigned non_loop_end = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            Action& op = aops[i];
            Distribution<StateID>& transition = transitions[i];

            EngineValueType t_value(this->get_action_cost(state_id, op));
            value_t self_loop = 0_vt;
            bool non_loop = false;

            for (const auto& [succ_id, prob] : transition) {
                if (succ_id == state_id) {
                    self_loop += prob;
                } else {
                    const StateInfo& succ_info = lookup_initialize(succ_id);
                    t_value += prob * succ_info.value;
                    non_loop = true;
                }
            }

            if (non_loop) {
                if (self_loop > 0_vt) {
                    t_value *= 1_vt / (1_vt - self_loop);
                }

                if (non_loop_end != i) {
                    aops[non_loop_end] = std::move(op);
                    transitions[non_loop_end] = std::move(transition);
                }

                ++non_loop_end;

                set_min(best_value, t_value);
                values.push_back(t_value);
            }
        }

        // erase self-loop states
        aops.erase(aops.begin() + non_loop_end, aops.end());
        transitions.erase(
            transitions.begin() + non_loop_end,
            transitions.end());

        return best_value;
    }

    EngineValueType compute_optimal_transitions(
        StateID state_id,
        StateInfo& state_info,
        std::vector<Action>& opt_aops,
        std::vector<Distribution<StateID>>& opt_transitions)
    {
        std::vector<EngineValueType> values;
        const EngineValueType best_value =
            compute_non_loop_transitions_and_values(
                state_id,
                state_info,
                opt_aops,
                opt_transitions,
                values);

        if (opt_aops.empty()) {
            return best_value;
        }

        unsigned optimal_end = 0;
        for (unsigned i = 0; i < opt_aops.size(); ++i) {
            if (is_approx_equal(
                    as_lower_bound(best_value),
                    as_lower_bound(values[i]))) {
                if (optimal_end != i) {
                    opt_aops[optimal_end] = std::move(opt_aops[i]);
                    opt_transitions[optimal_end] =
                        std::move(opt_transitions[i]);
                }

                ++optimal_end;
            }
        }

        // Erase non-optimal transitions
        opt_aops.erase(opt_aops.begin() + optimal_end, opt_aops.end());
        opt_transitions.erase(
            opt_transitions.begin() + optimal_end,
            opt_transitions.end());

        return best_value;
    }

    bool compute_value_update_and_optimal_transitions(
        StateID state_id,
        StateInfo& state_info,
        std::vector<Action>& opt_aops,
        std::vector<Distribution<StateID>>& opt_transitions)
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

        EngineValueType optimal_value = compute_optimal_transitions(
            state_id,
            state_info,
            opt_aops,
            opt_transitions);

        if (opt_aops.empty()) {
            statistics_.self_loop_states++;
            return notify_dead_end(state_info);
        }

        if (this->update(state_info, optimal_value)) {
            ++statistics_.value_changes;
            if (state_id == initial_state_id_) {
                statistics_.jump();
            }
            return true;
        }

        return false;
    }

    UpdateResult compute_value_policy_update(
        StateID state_id,
        Distribution<StateID>* greedy_transition)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        std::vector<Action> opt_aops;
        std::vector<Distribution<StateID>> opt_transitions;

        StateInfo& state_info = lookup_initialize(state_id);

        const bool value_changed = compute_value_update_and_optimal_transitions(
            state_id,
            state_info,
            opt_aops,
            opt_transitions);

        if (opt_aops.empty()) {
            state_info.clear_policy();
            return UpdateResult{value_changed, false};
        }

        auto [policy_changed, action] = compute_policy_update(
            state_id,
            state_info,
            opt_aops,
            opt_transitions,
            greedy_transition);

        return UpdateResult{value_changed, policy_changed, std::move(action)};
    }

    std::pair<bool, Action> compute_policy_update(
        StateID state_id,
        StateInfo& state_info,
        std::vector<Action>& opt_aops,
        std::vector<Distribution<StateID>>& opt_transitions,
        Distribution<StateID>* greedy_transition)
    {
#if defined(EXPENSIVE_STATISTICS)
        TimerScope scoped(statistics_.policy_selection_time);
#endif

        ++statistics_.policy_updates;

        const int index = this->policy_chooser_->pick_index(
            *this->get_state_space(),
            state_id,
            state_info.get_policy(),
            opt_aops,
            opt_transitions,
            *this);
        assert(utils::in_bounds(index, opt_aops));

        Action& action = opt_aops[index];

        if (greedy_transition != nullptr) {
            (*greedy_transition) = std::move(opt_transitions[index]);
        }

        return {state_info.update_policy(action), std::move(action)};
    }
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

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_BASE_H__