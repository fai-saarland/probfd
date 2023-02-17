#ifndef MDPS_ENGINES_HEURISTIC_SEARCH_BASE_H
#define MDPS_ENGINES_HEURISTIC_SEARCH_BASE_H

#include "probfd/engines/engine.h"
#include "probfd/engines/heuristic_search_state_information.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"
#include "probfd/engine_interfaces/new_state_handler.h"
#include "probfd/engine_interfaces/policy_picker.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/graph_visualization.h"

#include "probfd/progress_report.h"
#include "probfd/value_utils.h"

#include "utils/system.h"

#if defined(EXPENSIVE_STATISTICS)
#include "utils/timer.h"
#endif

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <limits>
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
template <typename StateT, typename ActionT, typename StateInfoT>
class HeuristicSearchBase
    : public MDPEngine<StateT, ActionT>
    , public engine_interfaces::HeuristicSearchInterface {
public:
    using State = StateT;
    using Action = ActionT;
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using IncumbentSolution = probfd::IncumbentSolution<UseInterval>;

    explicit HeuristicSearchBase(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::PolicyPicker<Action>* policy_chooser,
        engine_interfaces::NewStateHandler<State>* new_state_handler,
        engine_interfaces::StateEvaluator<State>* value_init,
        ProgressReport* report,
        bool interval_comparison,
        bool stable_policy)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              transition_generator,
              cost_function)
        , report_(report)
        , interval_comparison_(interval_comparison)
        , stable_policy_(stable_policy)
        , value_initializer_(value_init)
        , policy_chooser_(policy_chooser)
        , on_new_state_(new_state_handler)
    {
        statistics_.state_info_bytes = sizeof(StateInfo);
    }

    virtual ~HeuristicSearchBase() = default;

    virtual std::optional<value_t> get_error(const State& s) override
    {
        if constexpr (UseInterval) {
            const StateInfo& info = state_infos_[this->get_state_id(s)];
            return info.value.length();
        } else {
            return std::nullopt;
        }
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    const engines::heuristic_search::StateFlags&
    lookup_state_flags(StateID state_id) override
    {
        return state_infos_[state_id];
    }

    value_t lookup_value(StateID state_id) override
    {
        if constexpr (UseInterval) {
            return state_infos_[state_id].value.upper;
        } else {
            return state_infos_[state_id].value;
        }
    }

    Interval lookup_dual_bounds(StateID state_id) override
    {
        if constexpr (!UseInterval) {
            ABORT("Search algorithm does not support interval bounds!");
        } else {
            return state_infos_[state_id].value;
        }
    }

    ActionID lookup_policy(StateID state_id) override
    {
        if constexpr (!StorePolicy) {
            ABORT("Search algorithm does not store policy information!");
        } else {
            return state_infos_[state_id].policy;
        }
    }

    /**
     * @brief Checks if the state \p state_id is terminal.
     */
    bool is_terminal(StateID state_id)
    {
        return state_infos_[state_id].is_terminal();
    }

    /**
     * @brief Gets the current value of the state represented by \p state_id
     */
    value_t get_value(StateID state_id)
    {
        return as_upper_bound(state_infos_[state_id].value);
    }

    /**
     * @brief Chekcs if the state represented by \p state_id is marked as a
     * dead-end.
     */
    bool is_marked_dead_end(StateID state)
    {
        return state_infos_[state].is_dead_end();
    }

    /**
     * @brief Clears the currently selected greedy action for the state
     * represented by \p state_id
     */
    void clear_policy(StateID state_id)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        state_infos_[state_id].set_policy(ActionID::undefined);
    }

    /**
     * @brief Gets the currently selected greedy action for the state
     * represented by \p state_id
     */
    Action get_policy(StateID state_id)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        const ActionID aid = state_infos_[state_id].get_policy();
        assert(aid != ActionID::undefined);
        return this->lookup_action(state_id, aid);
    }

    /**
     * @brief Generates the successor distribution referring to the application
     * of the current greedy action in a state
     *
     * @param[in] state - The id of the source state
     * @param[out] result - The returned successor distribution when applying
     * the current greedy action in the state represented by \p state
     */
    bool apply_policy(StateID state, Distribution<StateID>& result)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        const StateInfo& info = state_infos_[state];
        if (info.policy == ActionID::undefined) {
            return async_update(state, nullptr, &result).first;
        }

        Action action = this->lookup_action(state, info.policy);
        this->generate_successors(state, action, result);
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
            state_info.value = IncumbentSolution(state_info.state_cost);
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
            return compute_value_update(s);
        } else {
            return async_update(s, nullptr, nullptr).first;
        }
    }

    /**
     * @brief Computes the value and policy update for a state and outputs the
     * new greedy action and transition.
     *
     * Output parameters may be nullptr. The first returns values specifies
     * whether the value changed. the second value specified whether the policy
     * changed.
     *
     * Only applicable if the policy is stored.
     *
     * @param[in] s - The state for the value update
     * @param[out] policy_action - Return address for the new greedy action.
     * @param[out] policy_transition - Return address for the new greedy
     * transition.
     */
    std::pair<bool, bool> async_update(
        StateID s,
        ActionID* policy_action,
        Distribution<StateID>* policy_transition)
    {
        return compute_value_policy_update(
            s,
            *this->policy_chooser_,
            stable_policy_,
            policy_action,
            policy_transition);
    }

    /**
     * @brief Computes the value and policy update for a state and outputs the
     * new greedy action and transition. Ties between optimal actions are
     * broken by the supplied policy tiebreaker.
     *
     * Output parameters may be nullptr. The first returns values specifies
     * whether the value changed. the second value specified whether the policy
     * changed.
     *
     * Only applicable if the policy is stored.
     *
     * @param[in] s - The state for the value update
     * @param[in] greedy_picker - A pointer to a function object breaking
     * ties between optimal actions.
     * @param[out] policy_action - Return address for the new greedy action.
     * @param[out] policy_transition - Return address for the new greedy
     * transition.
     */
    template <typename T>
    std::pair<bool, bool> async_update(
        StateID s,
        T& greedy_picker,
        ActionID* policy_action,
        Distribution<StateID>* policy_transition)
    {
        return compute_value_policy_update(
            s,
            greedy_picker,
            false,
            policy_action,
            policy_transition);
    }

protected:
    value_t get_state_cost(StateID id) { return get_state_info(id).state_cost; }

    value_t get_value(const State& s)
    {
        return get_value(this->get_state_id(s));
    }

    /**
     * @brief Updates the value of a state in its info object.
     *
     * @return true if the single state value changed by more than epsilon, the
     * lower bounding state value changed by more than epsilon (interval bounds
     * without interval comparison) or either value bound changed by more than
     * epsilon (interval bounds with interval comparison). False otherwise.
     */
    bool update(StateInfo& state_info, const IncumbentSolution& other)
    {
        if constexpr (UseInterval) {
            return probfd::update(
                state_info.value,
                other,
                interval_comparison_);
        } else {
            return probfd::update(state_info.value, other);
        }
    }

    /**
     * @brief Initializes the progress report with the given initial state.
     */
    void initialize_report(const State& state)
    {
        initial_state_id_ = this->get_state_id(state);

        static bool initialized = false;
        if (initialized) {
            return;
        }
        initialized = true;

        const StateInfo& info = lookup_initialize(this->get_state_id(state));
        this->add_values_to_report(&info);
        statistics_.value = as_upper_bound(info.value);
        statistics_.before_last_update = statistics_;
        statistics_.initial_state_estimate = as_upper_bound(info.value);
        statistics_.initial_state_found_terminal = info.is_terminal();

        setup_custom_reports(state);
    }

    /**
     * @brief Advances the progress report.
     */
    void report(const StateID) { this->report_->operator()(); }

    /**
     * @brief Sets up internal custom reports of a state in an implementation.
     */
    virtual void setup_custom_reports(const State&) {}

    /**
     * @brief Get the state info storage.
     */
    storage::PerStateStorage<StateInfo>& get_state_info_store()
    {
        return state_infos_;
    }

    /**
     * @brief Get the state info storage.
     */
    const storage::PerStateStorage<StateInfo>& get_state_info_store() const
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
    StateInfo& get_state_info(StateID id) const { return state_infos_[id]; }

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
     * @return false if interval bounds or interval comparison are not used.
     * Otherwise returns true if and only if the value bounds are epsilon-close.
     */
    template <typename Info>
    bool do_bounds_disagree(StateID state_id, const Info& info)
    {
        if constexpr (UseInterval) {
            if constexpr (std::is_same_v<Info, StateInfo>) {
                return interval_comparison_ &&
                       !info.value.bounds_approximately_equal();
            } else {
                return interval_comparison_ &&
                       !state_infos_[state_id]
                            .value.bounds_approximately_equal();
            }
        } else {
            return false;
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
        const std::function<std::string(const State&)> sstr =
            [](const State&){ return ""; })
    {
        struct ExpansionCondition
            : public engine_interfaces::StateEvaluator<State> {
            explicit ExpansionCondition(
                const MDPEngine<State, Action>* hs,
                storage::PerStateStorage<StateInfo>* infos)
                : hs_(hs)
                , infos_(infos)
            {
            }

            EvaluationResult operator()(const State& state) const override
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
            [](const Action&){ return ""; },
            &prune,
            false);
    }

private:
    void add_values_to_report(const StateInfo* info)
    {
        if constexpr (UseInterval) {
            report_->register_value("vl", [info]() {
                return as_lower_bound(info->value);
            });
            report_->register_value("vu", [info]() {
                return as_upper_bound(info->value);
            });
        } else {
            report_->register_value("v", [info]() {
                return as_upper_bound(info->value);
            });
        }
    }

    StateInfo& lookup_initialize(StateID state_id)
    {
        StateInfo& info = state_infos_[state_id];
        initialize(state_id, info);
        return info;
    }

    void initialize(StateID state_id, StateInfo& state_info)
    {
        if (!state_info.is_value_initialized()) {
            statistics_.evaluated_states++;

            State state = this->lookup_state(state_id);
            TerminationInfo term =
                MDPEngine<StateT, ActionT>::get_termination_info(state);
            const value_t t_cost = term.get_cost();

            state_info.state_cost = t_cost;
            if (term.is_goal_state()) {
                state_info.set_goal();
                state_info.value = IncumbentSolution(t_cost);
                statistics_.goal_states++;
                if (on_new_state_) on_new_state_->touch_goal(state);
                return;
            }

            EvaluationResult estimate = value_initializer_->evaluate(state);
            if (estimate.is_unsolvable()) {
                statistics_.pruned_states++;
                notify_dead_end(state_info);
                if (on_new_state_) on_new_state_->touch_dead_end(state);
            } else {
                state_info.set_on_fringe();

                if constexpr (UseInterval) {
                    state_info.value.upper = estimate.get_estimate();
                } else {
                    state_info.value = estimate.get_estimate();
                }

                if (on_new_state_) on_new_state_->touch(state);
            }
        }
    }

    bool compute_value_update(StateID state_id)
    {
        std::vector<Action> aops;
        std::vector<Distribution<StateID>> transitions;
        std::vector<IncumbentSolution> values;
        IncumbentSolution new_value;

        return compute_value_update(
            state_id,
            lookup_initialize(state_id),
            aops,
            transitions,
            values,
            new_value);
    }

    bool compute_value_update(
        StateID state_id,
        StateInfo& state_info,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& transitions,
        std::vector<IncumbentSolution>& values,
        IncumbentSolution& new_value)
    {
#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.resume();
#endif
        statistics_.backups++;

        if (state_info.is_terminal()) {
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            return false;
        }

        if (state_info.is_on_fringe()) {
            ++statistics_.backed_up_states;
            state_info.removed_from_fringe();
        }

        this->generate_all_successors(state_id, aops, transitions);
        assert(aops.size() == transitions.size());

        if (aops.empty()) {
            statistics_.terminal_states++;
            bool result = notify_dead_end(state_info);
#if defined(EXPENSIVE_STATISTICS)
            statistics_.update_time.stop();
#endif
            if (result) {
                ++statistics_.value_changes;
                if (state_id == initial_state_id_) {
                    statistics_.jump();
                }
            }
            return result;
        }

        new_value = IncumbentSolution(this->get_state_cost(state_id));
        values.reserve(aops.size());

        unsigned non_loop_end = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            Action& op = aops[i];
            Distribution<StateID>& transition = transitions[i];

            IncumbentSolution t_value(this->get_action_cost(state_id, op));
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

                values.push_back(t_value);
                set_min(new_value, t_value);

                if (non_loop_end != i) {
                    aops[non_loop_end] = std::move(op);
                    transitions[non_loop_end] = std::move(transition);
                }

                ++non_loop_end;
            }
        }

        aops.erase(aops.begin() + non_loop_end, aops.end());
        transitions.erase(
            transitions.begin() + non_loop_end,
            transitions.end());

#if defined(EXPENSIVE_STATISTICS)
        statistics_.update_time.stop();
#endif

        if (aops.empty()) {
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

    template <typename T>
    std::pair<bool, bool> compute_value_policy_update(
        StateID state_id,
        T& greedy_picker,
        bool stable_policy,
        ActionID* greedy_action,
        Distribution<StateID>* greedy_transition)
    {
        static_assert(StorePolicy, "Policy not stored by algorithm!");

        std::vector<Action> aops;
        std::vector<Distribution<StateID>> transitions;
        std::vector<IncumbentSolution> values;
        IncumbentSolution new_value;

        StateInfo& state_info = lookup_initialize(state_id);

        bool b = compute_value_update(
            state_id,
            state_info,
            aops,
            transitions,
            values,
            new_value);

        if (aops.empty()) {
            state_info.set_policy(ActionID::undefined);
            return std::make_pair(b, false);
        }

        bool p = compute_policy_update(
            state_id,
            state_info,
            greedy_picker,
            stable_policy,
            aops,
            transitions,
            values,
            new_value,
            greedy_action,
            greedy_transition);

        return std::make_pair(b, p);
    }

    template <typename T>
    bool compute_policy_update(
        StateID state_id,
        StateInfo& state_info,
        T& greedy_picker,
        bool stable,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& transitions,
        const std::vector<IncumbentSolution>& values,
        const IncumbentSolution& new_value,
        ActionID* greedy_action,
        Distribution<StateID>* greedy_transition)
    {
#if defined(EXPENSIVE_STATISTICS)
        statistics_.policy_selection_time.resume();
#endif
        auto previous_greedy = state_info.get_policy();

        unsigned optimal_end = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            if (approx_compare(values[i], new_value) <= 0) {
                if (stable) {
                    const auto aid = this->get_action_id(state_id, aops[i]);
                    if (aid == previous_greedy) {
                        if (greedy_action != nullptr) {
                            *greedy_action = aid;
                        }

                        if (greedy_transition != nullptr) {
                            *greedy_transition = std::move(transitions[i]);
                        }

                        return false;
                    }
                }

                if (i != optimal_end) {
                    transitions[optimal_end] = std::move(transitions[i]);
                    aops[optimal_end] = std::move(aops[i]);
                }

                ++optimal_end;
            }
        }

        aops.erase(aops.begin() + optimal_end, aops.end());
        transitions.erase(transitions.begin() + optimal_end, transitions.end());

        assert(!aops.empty() && !transitions.empty());

        ++statistics_.policy_updates;

        int index =
            greedy_picker
                .pick(state_id, previous_greedy, aops, transitions, *this);
        assert(index < 0 || index < static_cast<int>(aops.size()));

        if (index >= 0) {
            const ActionID aid = this->get_action_id(state_id, aops[index]);

            if (greedy_action != nullptr) {
                (*greedy_action) = aid;
            }

            if (greedy_transition != nullptr) {
                (*greedy_transition) = std::move(transitions[index]);
            }

            if (aid != state_info.get_policy()) {
                state_info.set_policy(aid);
                return true;
            }

#if defined(EXPENSIVE_STATISTICS)
            statistics_.policy_selection_time.stop();
#endif

            return false;
        }

#if defined(EXPENSIVE_STATISTICS)
        statistics_.policy_selection_time.stop();
#endif
        return false;
    }

protected:
    ProgressReport* report_;
    const bool interval_comparison_;
    const bool stable_policy_;

private:
    engine_interfaces::StateEvaluator<State>* value_initializer_;
    engine_interfaces::PolicyPicker<Action>* policy_chooser_;
    engine_interfaces::NewStateHandler<State>* on_new_state_;

    storage::PerStateStorage<StateInfo> state_infos_;

    Statistics statistics_;

    StateID initial_state_id_ = StateID::undefined;
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
    StateInfo<PerStateBaseInformation<StorePolicy, UseInterval>>>;

} // namespace heuristic_search
} // namespace engines
} // namespace probfd

#endif // __HEURISTIC_SEARCH_BASE_H__