#ifndef PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#define PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#include "probfd/algorithms/heuristic_search_state_information.h"
#include "probfd/algorithms/types.h"
#include "probfd/algorithms/utils.h"

#include "iterative_mdp_algorithm.h"
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
struct TransitionTail;
template <typename, typename>
class CostFunction;
} // namespace probfd

namespace probfd::algorithms {
template <typename, typename>
class PolicyPicker;
template <typename>
class SuccessorSampler;
} // namespace probfd::algorithms

namespace probfd::algorithms::fret {
template <typename, typename, typename, typename>
class FRET;
template <typename, typename, typename>
class PolicyGraph;
template <typename, typename, typename>
class ValueGraph;
} // namespace probfd::algorithms::fret

/// Namespace dedicated to the MDP h search base implementation
namespace probfd::algorithms::heuristic_search {

namespace internal {

/**
 * @brief Base statistics for MDP h search.
 */
struct Statistics {
    unsigned long long evaluated_states = 0;
    unsigned long long pruned_states = 0;
    unsigned long long goal_states = 0;

    unsigned long long expanded_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long self_loop_states = 0;

    unsigned long long value_changes = 0;
    unsigned long long policy_changes = 0;
    unsigned long long value_updates = 0;
    unsigned long long policy_updates = 0;

    value_t initial_state_estimate = 0;
    bool initial_state_found_terminal = false;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer update_time = utils::Timer(true);
    utils::Timer policy_selection_time = utils::Timer(true);
#endif

    /**
     * @brief Prints the statistics to the specified output stream.
     */
    void print(std::ostream& out) const;
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
    using CostFunctionType = CostFunction<State, Action>;
    using HeuristicType = Heuristic<State>;
    using TransitionTailType = TransitionTail<Action>;

    using PolicyPickerType = PolicyPicker<State, Action>;

    // Fret implementation has access to the internals of this base class.
    template <typename, typename, typename, typename>
    friend class fret::FRET;

    template <typename, typename, typename>
    friend class fret::PolicyGraph;

    template <typename, typename, typename>
    friend class fret::ValueGraph;

public:
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using AlgorithmValueType = AlgorithmValue<UseInterval>;

private:
    // Algorithm parameters
    const std::shared_ptr<PolicyPickerType> policy_chooser_;

protected:
    // Algorithm state
    internal::StateInfos<StateInfo> state_infos_;

    internal::Statistics statistics_;

public:
    explicit HeuristicSearchBase(
        std::shared_ptr<PolicyPickerType> policy_chooser);

    /**
     * @brief Looks up the current value interval of \p state_id.
     */
    [[nodiscard]]
    Interval lookup_bounds(StateID state_id) const;

    /**
     * @brief Checks if the state represented by \p state_id has been visited
     * yet.
     */
    [[nodiscard]]
    bool was_visited(StateID state_id) const;

    /**
     * @brief Computes the Bellman operator value for a state.
     */
    AlgorithmValueType compute_bellman(
        ParamType<State> source_state,
        const std::vector<TransitionTailType>& transition_tails,
        CostFunctionType& cost_function) const;

    /**
     * @brief Computes the Bellman operator value for a state, as well as all
     * transitions achieving a value epsilon-close to the minimum value and
     * their computed Q-values.
     *
     * @param[in, out] transition_tails The set of transition tails to compute
     * the Bellman operator for. The greedy transition tails are returned
     * through this parameter by erasing all non-greedy transitions.
     * All greedy transition tails will maintain their relative order.
     * If no transition achieves a value lower than the termination cost for
     * the source state, the empty list is returned, regardless of the epsilon
     * parameter.
     *
     * @param[out] qvalues The Q-values are added to this list, which must be
     * empty prior to the call, in the order that matches the greedy
     * transition tails returned in @p transition_tails .
     */
    AlgorithmValueType compute_bellman_and_greedy(
        ParamType<State> source_state,
        std::vector<TransitionTailType>& transition_tails,
        CostFunctionType& cost_function,
        std::vector<AlgorithmValueType>& qvalues,
        value_t epsilon) const;

    /**
     * @brief Selects a greedy transition from the given list of greedy
     * transitions through the policy selector passed on construction.
     *
     * If the transition list is empty, returns std::nullopt, otherwise some
     * transition from the list.
     *
     * @attention The selected transition is moved from the transition list
     * to avoid a copy.
     */
    std::optional<TransitionTailType> select_greedy_transition(
        MDPType& mdp,
        std::optional<Action> previous_greedy_action,
        std::vector<TransitionTailType>& greedy_transition_tails);

    /**
     * @brief Updates the value of the state associated with the given storage.
     *
     * @returns A pair indicating whether the value has changed by more than
     * the given epsilon in the first component and whether the value is
     * considered to be epsilon-close to optimal in the second component.
     */
    ValueUpdateResult update_value(
        StateInfo& state_info,
        AlgorithmValueType other,
        value_t epsilon);

    /**
     * @brief Updates the current greedy action of the state associated with
     * the given storage.
     *
     * Returns true if the greedy action has changed and false otherwise.
     */
    bool update_policy(
        StateInfo& state_info,
        const std::optional<TransitionTailType>& transition_tail)
        requires(StorePolicy);

protected:
    void initialize_initial_state(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state);

    void expand_and_initialize(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state,
        StateInfo& state_info,
        std::vector<TransitionTailType>& transition_tails);

    void generate_non_tip_transitions(
        MDPType& mdp,
        ParamType<State> state,
        std::vector<TransitionTailType>& transition_tails) const;

    void print_statistics(std::ostream& out) const;

private:
    void initialize(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state,
        StateInfo& state_info);

    AlgorithmValueType compute_qvalue(
        const TransitionTailType& transition_tail,
        CostFunctionType& cost_function) const;

    AlgorithmValueType compute_q_values(
        std::vector<TransitionTailType>& transition_tails,
        CostFunctionType& cost_function,
        std::vector<AlgorithmValueType>& qvalues) const;

    AlgorithmValueType filter_greedy_transitions(
        std::vector<TransitionTailType>& transition_tails,
        std::vector<AlgorithmValueType>& qvalues,
        const AlgorithmValueType& best_value,
        value_t epsilon) const;
};

/**
 * @brief Extends HeuristicSearchBase with default implementations for
 * MDPAlgorithm.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class HeuristicSearchAlgorithm
    : public IterativeMDPAlgorithm<State, Action>
    , public HeuristicSearchBase<State, Action, StateInfoT> {
    using AlgorithmBase = typename HeuristicSearchAlgorithm::MDPAlgorithm;
    using HSBase = typename HeuristicSearchAlgorithm::HeuristicSearchBase;

public:
    using TransitionTailType = HSBase::TransitionTailType;
    using AlgorithmValueType = HSBase::AlgorithmValueType;

protected:
    using PolicyType = typename AlgorithmBase::PolicyType;

    using MDPType = typename AlgorithmBase::MDPType;
    using HeuristicType = typename AlgorithmBase::HeuristicType;

    using StateInfo = typename HSBase::StateInfo;
    using PolicyPicker = typename HSBase::PolicyPickerType;

public:
    HeuristicSearchAlgorithm(
        value_t epsilon,
        std::shared_ptr<PolicyPicker> policy_chooser);

    Interval solve(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) final;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) final;

    void print_statistics(std::ostream& out) const final;

    /**
     * @brief Solves for the optimal state value of the input state.
     *
     * Called internally after initializing the progress report.
     */
    virtual Interval do_solve(
        MDPType& mdp,
        HeuristicType& h,
        ParamType<State> state,
        ProgressReport& progress,
        double max_time) = 0;

    /**
     * @brief Prints additional statistics to the output stream.
     *
     * Called internally after printing the base h search statistics.
     */
    virtual void print_additional_statistics(std::ostream& out) const = 0;
};

/**
 * @brief Heuristics search algorithm that can be used within FRET.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the undelying MDP model.
 * @tparam StateInfoT - The state information container type.
 */
template <typename State, typename Action, typename StateInfoT>
class FRETHeuristicSearchAlgorithm
    : public HeuristicSearchAlgorithm<State, Action, StateInfoT> {
    using AlgorithmBase = typename FRETHeuristicSearchAlgorithm::MDPAlgorithm;
    using HSBase =
        typename FRETHeuristicSearchAlgorithm::HeuristicSearchAlgorithm;

protected:
    using PolicyType = typename AlgorithmBase::PolicyType;

    using MDPType = typename AlgorithmBase::MDPType;
    using HeuristicType = typename AlgorithmBase::HeuristicType;

    using StateInfo = typename HSBase::StateInfo;
    using PolicyPicker = typename HSBase::PolicyPickerType;

public:
    // Inherited constructor
    using HSBase::HSBase;

    /**
     * @brief Resets the h search algorithm object to a clean state.
     *
     * This method is needed by the FRET algorithm to restart the heuristic
     * search after traps have been collapsed.
     */
    virtual void reset_search_state() {}
};

} // namespace probfd::algorithms::heuristic_search

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#include "probfd/algorithms/heuristic_search_base_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#endif // __HEURISTIC_SEARCH_BASE_H__