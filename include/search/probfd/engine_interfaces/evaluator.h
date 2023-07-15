#ifndef PROBFD_ENGINE_INTERFACES_EVALUATOR_H
#define PROBFD_ENGINE_INTERFACES_EVALUATOR_H

#include "probfd/type_traits.h"
#include "probfd/types.h"
#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Specifies the heuristic estimate and solvability status of a state.
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

    /// Checks whether the state is unsolvable.
    bool is_unsolvable() const { return is_unsolvable_; }

    /// Gets the heuristic estimate of the state.
    value_t get_estimate() const { return estimate_; }
};

namespace engine_interfaces {

/**
 * @brief The interface representing heuristic functions.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class Evaluator {
public:
    virtual ~Evaluator() = default;

    /**
     * @brief Evaluates the heuristic on a given state and returns the
     * solvability and heuristic value estimates.
     *
     * @see EvaluationResult
     */
    virtual EvaluationResult evaluate(param_type<State> state) const = 0;

    /**
     * @brief Prints statistics, e.g. the number of queries made to the
     * interface.
     */
    virtual void print_statistics() const {}
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __STATE_EVALUATOR_H__