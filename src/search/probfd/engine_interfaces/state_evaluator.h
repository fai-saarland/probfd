#ifndef MDPS_ENGINE_INTERACES_STATE_EVALUATOR_H
#define MDPS_ENGINE_INTERACES_STATE_EVALUATOR_H

#include "../evaluation_result.h"

namespace probabilistic {

/**
 * @brief Function object specifying the immediate reward received in a target
 * state, independent of the source state and action taken.
 *
 * @tparam State - The state type of the underlying MDP model.
 *
 * @see EvaluationResult
 */
template <typename State>
class StateEvaluator {
public:
    virtual ~StateEvaluator() = default;

    EvaluationResult operator()(const State& state) const
    {
        return this->evaluate(state);
    }

    virtual void print_statistics() const {}

protected:
    virtual EvaluationResult evaluate(const State& state) const = 0;
};

} // namespace probabilistic

#endif // __STATE_EVALUATOR_H__