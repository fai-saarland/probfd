#ifndef PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#define PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#include "probfd/algorithms/heuristic_search_state_information.h"
#include "probfd/algorithms/types.h"

#include "probfd/mdp_algorithm.h"
#include "probfd/progress_report.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <algorithm>
#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>

// Forward Declarations
namespace probfd {
template <typename>
class Distribution;
template <typename>
struct Transition;
} // namespace probfd

namespace probfd::algorithms {
template <typename, typename>
class PolicyPicker;
template <typename>
class SuccessorSampler;
} // namespace probfd::algorithms

/// Namespace dedicated to the MDP h search base implementation
namespace probfd::algorithms::heuristic_search {

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
    bool initial_state_found_terminal = false;

    value_t value = 0_vt;
    CoreStatistics before_last_update;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer update_time = utils::Timer(true);
    utils::Timer policy_selection_time = utils::Timer(true);
#endif

    /**
     * @brief Prints the statistics to the specified output stream.
     */
    void print(std::ostream& out) const;

    void jump()
    {
        before_last_update = static_cast<const CoreStatistics&>(*this);
    }
};

template <typename StateInfo>
class StateInfos : public StateProperties {
    storage::PerStateStorage<StateInfo> state_infos_;

public:
    StateInfo& operator[](StateID sid) { return state_infos_[sid]; }
    const StateInfo& operator[](StateID sid) const { return state_infos_[sid]; }

    value_t lookup_value(StateID state_id) override
    {
        return state_infos_[state_id].get_value();
    }

    Interval lookup_bounds(StateID state_id) override
    {
        return state_infos_[state_id].get_bounds();
    }

    void reset() { std::ranges::for_each(state_infos_, &StateInfo::clear); }
};

} // namespace internal

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
    using MDPType = MDP<State, Action>;
    using EvaluatorType = Evaluator<State>;
    using TransitionType = Transition<Action>;

    using PolicyPickerType = PolicyPicker<State, Action>;

public:
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using AlgorithmValueType = AlgorithmValue<UseInterval>;

private:
    // Algorithm parameters
    const std::shared_ptr<PolicyPickerType> policy_chooser_;

    StateInfo* initial_state_info_ = nullptr;

    // Reused buffer
    std::vector<TransitionType> transitions_;

protected:
    // Algorithm state
    internal::StateInfos<StateInfo> state_infos_;

    internal::Statistics statistics_;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
        std::optional<TransitionType> greedy_transition = std::nullopt;
    };

public:
    explicit HeuristicSearchBase(
        std::shared_ptr<PolicyPickerType> policy_chooser);

    /**
     * @brief Looks up the current lower bound for the cost of \p state_id.
     */
    [[nodiscard]]
    value_t lookup_value(StateID state_id) const;

    /**
     * @brief Looks up the current cost interval of \p state_id.
     */
    [[nodiscard]]
    Interval lookup_bounds(StateID state_id) const;

    /**
     * @brief Checks if the state \p state_id is terminal.
     */
    [[nodiscard]]
    bool is_terminal(StateID state_id) const;

    /**
     * @brief Checks if the state represented by \p state_id has been visited
     * yet.
     */
    [[nodiscard]]
    bool was_visited(StateID state_id) const;

    /**
     * @brief Clears the currently selected greedy action for the state
     * represented by \p state_id
     */
    void clear_policy(StateID state_id)
        requires(StorePolicy);

    std::optional<Action> get_greedy_action(StateID state_id)
        requires(StorePolicy);

    std::optional<Action>
    compute_greedy_action(MDPType& mdp, EvaluatorType& h, StateID state_id)
        requires(!StorePolicy);

    /**
     * @brief Computes the Bellman update for a state and returns whether the
     * value changed.
     */
    bool bellman_update(MDPType& mdp, EvaluatorType& h, StateID s);

    /**
     * @brief Computes the Bellman update for a state and outputs all greedy
     * transitions, and returns whether the value changed.
     */
    bool bellman_update(
        MDPType& mdp,
        EvaluatorType& h,
        StateID state_id,
        std::vector<TransitionType>& greedy);

    /**
     * @brief Computes the Bellman update for a state, recomputes the greedy
     * action for it, and outputs status changes and the new greedy transition.
     */
    UpdateResult bellman_policy_update(
        MDPType& mdp,
        EvaluatorType& h,
        StateID state_id,
        StateInfo& state_info)
        requires(StorePolicy);

protected:
    void initialize_report(
        MDPType& mdp,
        EvaluatorType& h,
        param_type<State> state,
        ProgressReport& progress);

    void print_statistics(std::ostream& out) const;

private:
    // Stores dead-end information in state info.
    void set_dead_end(StateInfo& state_info, value_t termination_cost);

    bool update(StateInfo& state_info, AlgorithmValueType other);

    void state_value_changed(StateInfo& info);

    StateInfo&
    lookup_initialize(MDPType& mdp, EvaluatorType& h, StateID state_id);

    void initialize(
        MDPType& mdp,
        EvaluatorType& h,
        param_type<State> state,
        StateInfo& state_info);

    std::optional<AlgorithmValueType> normalized_qvalue(
        MDPType& mdp,
        EvaluatorType& h,
        StateID state_id,
        const TransitionType& transition);

    AlgorithmValueType compute_q_values(
        MDPType& mdp,
        EvaluatorType& h,
        StateID state_id,
        value_t termination_cost,
        std::vector<TransitionType>& transitions,
        std::vector<AlgorithmValueType>& qvalues);

    AlgorithmValueType filter_greedy_transitions(
        std::vector<TransitionType>& transitions,
        std::vector<AlgorithmValueType>& qvalues,
        const AlgorithmValueType& best_value);

    bool bellman_update(
        MDPType& mdp,
        EvaluatorType& h,
        StateID state_id,
        StateInfo& state_info,
        std::vector<TransitionType>& transitions,
        bool filter_greedy);
};

/**
 * @brief Extends HeuristicSearchBase with default implementations for
 * MDPAlgorithm.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the undelying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class HeuristicSearchAlgorithm
    : public MDPAlgorithm<State, Action>
    , public HeuristicSearchBase<State, Action, StateInfoT> {
    using AlgorithmBase = typename HeuristicSearchAlgorithm::MDPAlgorithm;
    using HSBase = typename HeuristicSearchAlgorithm::HeuristicSearchBase;

protected:
    using PolicyType = typename AlgorithmBase::PolicyType;

    using MDPType = typename AlgorithmBase::MDPType;
    using EvaluatorType = typename AlgorithmBase::EvaluatorType;

    using StateInfo = typename HSBase::StateInfo;
    using PolicyPicker = typename HSBase::PolicyPickerType;

public:
    // Inherited constructor
    using HSBase::HSBase;

    Interval solve(
        MDPType& mdp,
        EvaluatorType& h,
        param_type<State> state,
        ProgressReport progress,
        double max_time) final;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        EvaluatorType& h,
        param_type<State> state,
        ProgressReport progress,
        double max_time) final;

    void print_statistics(std::ostream& out) const final;

    /**
     * @brief Sets up internal custom reports of a state in an implementation.
     */
    virtual void setup_custom_reports(param_type<State>, ProgressReport&) {}

    /**
     * @brief Resets the h search algorithm object to a clean state.
     *
     * This method is needed by the FRET algorithm to restart the heuristic
     * search after traps have been collapsed.
     */
    virtual void reset_search_state() {}

    /**
     * @brief Solves for the optimal state value of the input state.
     *
     * Called internally after initializing the progress report.
     */
    virtual Interval do_solve(
        MDPType& mdp,
        EvaluatorType& h,
        param_type<State> state,
        ProgressReport& progress,
        double max_time) = 0;

    /**
     * @brief Prints additional statistics to the output stream.
     *
     * Called internally after printing the base h search statistics.
     */
    virtual void print_additional_statistics(std::ostream& out) const = 0;
};

} // namespace probfd::algorithms::heuristic_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#include "probfd/algorithms/heuristic_search_base_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#endif // __HEURISTIC_SEARCH_BASE_H__