#ifndef PROBFD_MDP_ALGORITHM_H
#define PROBFD_MDP_ALGORITHM_H

#include "probfd/policies/empty_policy.h"

#include "probfd/progress_report.h"
#include "probfd/type_traits.h"

#include <limits>
#include <memory>

// Forward Declarations
namespace probfd {
template <typename, typename>
class MDP;
template <typename>
class Evaluator;
class ProgressReport;
} // namespace probfd

namespace probfd {

/**
 * @brief Interface for MDP algorithm implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class MDPAlgorithm {
protected:
    using PolicyType = Policy<State, Action>;

    using MDPType = MDP<State, Action>;
    using EvaluatorType = Evaluator<State>;

    static constexpr double INF_TIME = std::numeric_limits<double>::infinity();

public:
    virtual ~MDPAlgorithm() = default;

    /**
     * @brief Computes a partial policy for the input state.
     *
     * The default implementation of this method returns an empty policy.
     */
    virtual std::unique_ptr<PolicyType> compute_policy(
        MDPType&,
        EvaluatorType&,
        param_type<State>,
        ProgressReport,
        double = INF_TIME)
    {
        return std::make_unique<policies::EmptyPolicy<State, Action>>();
    }

    /**
     * @brief Runs the MDP algorithm for the initial state \p state with a
     * maximum time limit.
     */
    virtual Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time = INF_TIME) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

} // namespace probfd

#endif // PROBFD_MDP_ALGORITHM_H