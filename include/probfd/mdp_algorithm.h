#ifndef PROBFD_MDP_ALGORITHM_H
#define PROBFD_MDP_ALGORITHM_H

#include "probfd/mdp_algorithm_fwd.h"

#include "probfd/policy.h"
#include "probfd/progress_report.h"
#include "probfd/type_traits.h"

#include <limits>
#include <memory>

// Forward Declarations
namespace probfd {
template <typename, typename>
class MDP;
template <typename>
class Heuristic;
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
    using HeuristicType = Heuristic<State>;

public:
    virtual ~MDPAlgorithm() = default;

    /**
     * @brief Computes a partial policy for the input state.
     */
    virtual std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

} // namespace probfd

#endif // PROBFD_MDP_ALGORITHM_H