#ifndef MDPS_ENGINE_INTERFACES_REWARD_FUNCTION_H
#define MDPS_ENGINE_INTERFACES_REWARD_FUNCTION_H

#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Contains reward received upon termination in a state, and whether
 * the state is a goal.
 */
class TerminationInfo {
    bool is_goal_;
    value_type::value_t terminal_reward_;

public:
    TerminationInfo() = default;

    TerminationInfo(bool is_goal, value_type::value_t terminal_reward)
        : is_goal_(is_goal)
        , terminal_reward_(terminal_reward)
    {
    }

    /// Is this state a goal?
    bool is_goal_state() const { return is_goal_; }

    /// Obtains the reward received upon termination in the state.
    value_type::value_t get_reward() const { return terminal_reward_; }
};

namespace engine_interfaces {

/**
 * @brief Interface specifying state termination rewards, action rewards and
 * goal states.
 *
 * This interface communicates the state and action rewards and goal states to
 * the MDP engines. Users must implement the protected methods
 * `evaluate(const State& state)` and
 * `evaluate(const State& state, const Action& action)`.
 *
 * Example
 * =======
 *
 * ```
class MaxProbStateReward :
public RewardFunction<State, OperatorID>
{
protected:
    StateReward evaluate(const GlobalState& state) override
    {
        bool is_goal = ...;

        // Receive a reward in goal states, no reward otherwise.
        return is_goal ?
            StateReward(true, value_type::one) :
            StateReward(false, value_type::zero);
    }

    StateReward evaluate(const State& state, OperatorID) override
    {
        // Actions do not give a reward.
        return 0;
    }
};
 * ```
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class RewardFunction {
public:
    virtual ~RewardFunction() = default;

    /**
     * @brief Get the terminal state reward of the input state.
     */
    TerminationInfo operator()(const State& state)
    {
        return this->evaluate(state);
    }

    /**
     * @brief Get the action reward of the action when applied in a state.
     */
    value_type::value_t operator()(StateID state_id, Action action)
    {
        return this->evaluate(state_id, action);
    }

    /**
     * @brief Prints statistics.
     */
    virtual void print_statistics() const {}

protected:
    virtual TerminationInfo evaluate(const State& state) = 0;
    virtual value_type::value_t evaluate(StateID state, Action action) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __STATE_REWARD_FUNCTION_H__