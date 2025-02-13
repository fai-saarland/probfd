#ifndef PROBFD_MDP_H
#define PROBFD_MDP_H

#include "probfd/cost_function.h" // IWYU pragma: export
#include "probfd/state_space.h"   // IWYU pragma: export

namespace probfd {

/**
 * @brief Basic interface for MDPs.
 */
template <typename State, typename Action>
class MDP
    : public StateSpace<State, Action>
    , public CostFunction<State, Action> {
    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics() const {}
};

/**
 * @brief Basic interface for MDPs.
 */
template <typename State, typename Action>
class SimpleMDP : public MDP<State, Action> {
public:
    TerminationInfo get_termination_info(ParamType<State> state) final
    {
        return is_goal(state)
                   ? TerminationInfo::from_goal(get_goal_termination_cost())
                   : TerminationInfo::from_non_goal(
                         get_non_goal_termination_cost());
    }

    virtual bool is_goal(ParamType<State> state) const = 0;
    [[nodiscard]]
    virtual value_t get_goal_termination_cost() const = 0;
    virtual value_t get_non_goal_termination_cost() const = 0;
};

/**
 * @brief Composes a state space and a cost function to an MDP.
 *
 * @tparam State - The state type of the MDP.
 * @tparam Action - The action type of the MDP.
 */
template <typename State, typename Action>
struct CompositeMDP : public MDP<State, Action> {
    using TransitionTailType = TransitionTail<Action>;

    StateSpace<State, Action>& state_space;
    CostFunction<State, Action>& cost_function;

    CompositeMDP(
        StateSpace<State, Action>& state_space,
        CostFunction<State, Action>& cost_function)
        : state_space(state_space)
        , cost_function(cost_function)
    {
    }

    /**
     * @brief Get the state ID for a given state.
     */
    StateID get_state_id(ParamType<State> state) final
    {
        return state_space.get_state_id(state);
    }

    /**
     * @brief Get the state mapped to a given state ID.
     */
    State get_state(StateID state_id) final
    {
        return state_space.get_state(state_id);
    }

    /**
     * @brief Generates the applicable actions of the state.
     */
    void generate_applicable_actions(
        ParamType<State> state,
        std::vector<Action>& result) final
    {
        return state_space.generate_applicable_actions(state, result);
    }

    /**
     * @brief Generates the successor distribution for a given state and action.
     */
    void generate_action_transitions(
        ParamType<State> state,
        ParamType<Action> action,
        SuccessorDistribution& successor_dist) final
    {
        return state_space.generate_action_transitions(
            state,
            action,
            successor_dist);
    }

    /**
     * @brief Generates all applicable actions and their corresponding successor
     * distributions for a given state.
     */
    void generate_all_transitions(
        ParamType<State> state,
        std::vector<Action>& aops,
        std::vector<SuccessorDistribution>& successor_dist) final
    {
        return state_space.generate_all_transitions(
            state,
            aops,
            successor_dist);
    }

    virtual void generate_all_transitions(
        ParamType<State> state,
        std::vector<TransitionTailType>& transitions) final
    {
        return state_space.generate_all_transitions(state, transitions);
    }

    /**
     * @brief Returns the cost to terminate in a given state and checks whether
     * a state is a goal
     *
     * @see TerminationInfo
     */
    TerminationInfo get_termination_info(ParamType<State> state) final
    {
        return cost_function.get_termination_info(state);
    }

    /**
     * @brief Gets the action cost of a state-action.
     */
    value_t get_action_cost(ParamType<Action> action) final
    {
        return cost_function.get_action_cost(action);
    }
};

} // namespace probfd

#endif