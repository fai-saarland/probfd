#ifndef PROBFD_ENGINE_H
#define PROBFD_ENGINE_H

#include "probfd/policies/empty_policy.h"

#include "probfd/distribution.h"
#include "probfd/evaluator.h"
#include "probfd/interval.h"
#include "probfd/mdp.h"

#include <limits>
#include <memory>
#include <optional>
#include <vector>

class State;
class OperatorID;

namespace probfd {

/**
 * @brief Interface for MDP engine implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class MDPEngine {
public:
    virtual ~MDPEngine() = default;

    /**
     * @brief Computes a partial policy for the input state.
     *
     * The default implementation of this method returns an empty policy.
     */
    virtual std::unique_ptr<PartialPolicy<State, Action>> compute_policy(
        MDP<State, Action>&,
        Evaluator<State>&,
        param_type<State>,
        double = std::numeric_limits<double>::infinity())
    {
        return std::make_unique<policies::EmptyPolicy<State, Action>>();
    }

    /**
     * @brief Runs the MDP algorithm for the initial state \p state with a
     * maximum time limit.
     */
    virtual Interval solve(
        MDP<State, Action>& mdp,
        Evaluator<State>& heuristic,
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

} // namespace probfd

#endif // __ENGINE_H__