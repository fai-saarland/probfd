#ifndef PROBFD_ITERATIVE_MDP_ALGORITHM_H
#define PROBFD_ITERATIVE_MDP_ALGORITHM_H

#include "probfd/mdp_algorithm.h"

namespace probfd {

/**
 * @brief Base class for iterative MDP algorithm implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class IterativeMDPAlgorithm : public MDPAlgorithm<State, Action> {
protected:
    using PolicyType = Policy<State, Action>;

    using MDPType = MDP<State, Action>;
    using HeuristicType = Heuristic<State>;

    /// Precision value for convergence checks
    const value_t epsilon;

public:
    /// Constructs the algorithm with the specified precision parameter for
    /// convergence checks.
    explicit IterativeMDPAlgorithm(value_t epsilon)
        : epsilon(epsilon)
    {
    }

    /// Get the tolerance for convergence checks.
    [[nodiscard]]
    value_t get_convergence_epsilon() const
    {
        return this->epsilon;
    }
};

} // namespace probfd

#endif // PROBFD_MDP_ALGORITHM_H