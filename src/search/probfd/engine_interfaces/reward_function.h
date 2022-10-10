#ifndef MDPS_ENGINE_INTERACES_REWARD_FUNCTION_H
#define MDPS_ENGINE_INTERACES_REWARD_FUNCTION_H

#include "../evaluation_result.h"
#include "../types.h"

namespace probfd {
namespace engine_interfaces {

/**
 * @brief Interface specifying state and action rewards and goal states.
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
public RewardFunction<GlobalState, const ProbabilisticOperator*>
{
protected:
    EvaluationResult evaluate(const GlobalState& state) override
    {
        bool is_goal = ::test_goal(state);

        // Receive a reward in goal states, no reward otherwise.
        return is_goal ?
            EvaluationResult(true, value_type::one) :
            EvaluationResult(false, value_type::zero);
    }

    EvaluationResult evaluate(
        const GlobalState& state,
        const ProbabilisticOperator* const& action) override
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
     * @brief Get the state reward and the goal state status of the input state.
     */
    EvaluationResult operator()(const State& state)
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
    virtual EvaluationResult evaluate(const State& state) = 0;
    virtual value_type::value_t evaluate(StateID state, Action action) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __STATE_REWARD_FUNCTION_H__