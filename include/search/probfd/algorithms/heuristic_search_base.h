#ifndef PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#define PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#include "probfd/algorithms/heuristic_search_state_information.h"
#include "probfd/algorithms/types.h"

#include "probfd/mdp_algorithm.h"
#include "probfd/progress_report.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>

namespace probfd {

template <typename>
class Distribution;

template <typename>
struct Transition;

namespace algorithms {

template <typename, typename>
class PolicyPicker;

template <typename>
class SuccessorSampler;

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
    void print(std::ostream& out) const;

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
    using Transition = Transition<Action>;

    using PolicyPicker = PolicyPicker<State, Action>;

public:
    using StateInfo = StateInfoT;

    static constexpr bool StorePolicy = StateInfo::StorePolicy;
    static constexpr bool UseInterval = StateInfo::UseInterval;

    using AlgorithmValueType = AlgorithmValueType<UseInterval>;

private:
    class StateInfos : public StateProperties {
        storage::PerStateStorage<StateInfo> state_infos_;

    public:
        StateInfo& operator[](StateID sid) { return state_infos_[sid]; }
        const StateInfo& operator[](StateID sid) const
        {
            return state_infos_[sid];
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

    StateInfos state_infos_;

    StateInfo* initial_state_info = nullptr;

    // Reused buffer
    std::vector<Transition> transitions_;

protected:
    Statistics statistics_;
    ProgressReport* report_;
    const bool interval_comparison_;

    struct UpdateResult {
        bool value_changed;
        bool policy_changed;
        std::optional<Transition> greedy_transition = std::nullopt;
    };

public:
    HeuristicSearchBase(
        std::shared_ptr<PolicyPicker> policy_chooser,
        ProgressReport* report,
        bool interval_comparison);

    /**
     * @brief Looks up the current lower bound for the cost of \p state_id.
     */
    value_t lookup_value(StateID state_id) const;

    /**
     * @brief Looks up the current cost interval of \p state_id.
     */
    Interval lookup_bounds(StateID state_id) const;

    /**
     * @brief Checks if the state \p state_id is terminal.
     */
    bool is_terminal(StateID state_id) const;

    /**
     * @brief Checks if the state represented by \p state_id is marked as a
     * dead-end.
     */
    bool is_marked_dead_end(StateID state_id) const;

    /**
     * @brief Checks if the state represented by \p state_id has been visited
     * yet.
     */
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
    compute_greedy_action(MDP& mdp, Evaluator& h, StateID state_id)
        requires(!StorePolicy);

    /**
     * @brief Computes the Bellman update for a state and returns whether the
     * value changed.
     */
    bool bellman_update(MDP& mdp, Evaluator& h, StateID s);

    /**
     * @brief Computes the Bellman update for a state and outputs all greedy
     * transitions, and returns whether the value changed.
     */
    bool bellman_update(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        std::vector<Transition>& greedy);

    /**
     * @brief Computes the Bellman update for a state, recomputes the greedy
     * action for it, and outputs status changes and the new greedy transition.
     */
    UpdateResult bellman_policy_update(MDP& mdp, Evaluator& h, StateID state_id)
        requires(StorePolicy);

protected:
    void initialize_report(MDP& mdp, Evaluator& h, param_type<State> state);

    void print_statistics(std::ostream& out) const;

    /**
     * @brief Advances the progress report.
     */
    void print_progress();

    bool check_interval_comparison() const;

    /**
     * @brief Get the state info object of a state.
     */
    StateInfo& get_state_info(StateID id);

    /**
     * @brief Get the state info object of a state.
     */
    const StateInfo& get_state_info(StateID id) const;

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
    StateInfo& get_state_info(StateID id, AlgStateInfo& info);

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
    const StateInfo& get_state_info(StateID id, const AlgStateInfo& info) const;

    StateID sample_state(
        SuccessorSampler<Action>& sampler,
        StateID source,
        const Distribution<StateID>& transition);

private:
    // Stores dead-end information in state info and returns true on change.
    bool notify_dead_end(StateInfo& state_info, value_t termination_cost);

    bool update(StateInfo& state_info, AlgorithmValueType other);

    void state_value_changed(StateInfo& info);

    StateInfo& lookup_initialize(MDP& mdp, Evaluator& h, StateID state_id);

    bool initialize_if_needed(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info);

    std::optional<AlgorithmValueType> normalized_qvalue(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        const Transition& transition);

    AlgorithmValueType filter_greedy_transitions(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        std::vector<Transition>& transitions,
        value_t termination_cost);

    bool bellman_update(
        MDP& mdp,
        Evaluator& h,
        StateID state_id,
        StateInfo& state_info,
        auto&... optional_out_greedy);
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
    using Policy = typename AlgorithmBase::Policy;

    using MDP = typename AlgorithmBase::MDP;
    using Evaluator = typename AlgorithmBase::Evaluator;

    using StateInfo = typename HSBase::StateInfo;
    using PolicyPicker = typename HSBase::PolicyPicker;

public:
    // Inherited constructor
    using HSBase::HSBase;

    Interval solve(
        MDP& mdp,
        Evaluator& h,
        param_type<State> state,
        double max_time =
            std::numeric_limits<double>::infinity()) final override;

    std::unique_ptr<Policy> compute_policy(
        MDP& mdp,
        Evaluator& h,
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) override;

    void print_statistics(std::ostream& out) const final override;

    /**
     * @brief Sets up internal custom reports of a state in an implementation.
     */
    virtual void setup_custom_reports(param_type<State>) {}
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
using HeuristicSearchAlgorithm = internal::HeuristicSearchAlgorithm<
    State,
    Action,
    StateInfo<PerStateBaseInformation<Action, StorePolicy, UseInterval>>>;

} // namespace heuristic_search
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H
#include "probfd/algorithms/heuristic_search_base_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_HEURISTIC_SEARCH_BASE_H

#endif // __HEURISTIC_SEARCH_BASE_H__