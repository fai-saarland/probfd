#ifndef MDPS_ENGINE_INTERACES_STATE_REWARD_FUNCTION_H
#define MDPS_ENGINE_INTERACES_STATE_REWARD_FUNCTION_H

#include "../evaluation_result.h"

namespace probabilistic {

/**
 * @brief Interface specifying state rewards and goal states.
 *
 * This interface communicates the state rewards and goal states to the
 * MDP engines. Users must implement the protected method
 * `evaluate(const State& state)`.
 *
 * Example
 * =======
 *
 * ```
class MaxProbStateReward : public StateRewardFunction<GlobalState> {
protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override
    {
        bool is_goal = ::test_goal(state);

        // Receive a reward in goal states, no reward otherwise.
        return is_goal ?
            EvaluationResult(true, value_type::one) :
            EvaluationResult(false, value_type::zero);
    }
};
 * ```
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class StateRewardFunction {
public:
    virtual ~StateRewardFunction() = default;

    /**
     * @brief Get the state reward and the goal state status of the input state.
     */
    EvaluationResult operator()(const State& state)
    {
        return this->evaluate(state);
    }

    /**
     * @brief Prints statistics.
     */
    virtual void print_statistics() const {}

protected:
    virtual EvaluationResult evaluate(const State& state) = 0;
};

} // namespace probabilistic

#endif // __STATE_REWARD_FUNCTION_H__