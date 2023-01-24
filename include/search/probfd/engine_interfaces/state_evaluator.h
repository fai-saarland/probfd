#ifndef MDPS_ENGINE_INTERFACES_STATE_EVALUATOR_H
#define MDPS_ENGINE_INTERFACES_STATE_EVALUATOR_H

#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Contains the heuristic estimate of the optimal state value and whether
 * the state is solvable.
 */
class EvaluationResult {
    bool is_unsolvable_;
    value_t estimate_;

public:
    EvaluationResult() = default;

    EvaluationResult(bool is_unsolvable, value_t estimate)
        : is_unsolvable_(is_unsolvable)
        , estimate_(estimate)
    {
    }

    /// Is this state unsolvable?
    bool is_unsolvable() const { return is_unsolvable_; }

    /// Converts to a value_t representing the state value.
    value_t get_estimate() const { return estimate_; }
};

namespace engine_interfaces {

/**
 * @brief Represents a heuristic function.
 *
 * @tparam State - The state type of the underlying MDP model.
 *
 * @see EvaluationResult
 */
template <typename State>
class StateEvaluator {
public:
    virtual ~StateEvaluator() = default;

    virtual EvaluationResult evaluate(const State& state) const = 0;

    virtual void print_statistics() const {}
};

} // namespace engine_interfaces
} // namespace probfd

class State;

namespace probfd {

/// Type alias for state reward functions for planning tasks.
using TaskStateEvaluator = engine_interfaces::StateEvaluator<State>;

} // namespace probfd

#endif // __STATE_EVALUATOR_H__