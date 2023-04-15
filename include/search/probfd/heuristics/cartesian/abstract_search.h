#ifndef PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_SEARCH_H
#define PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_SEARCH_H

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition.h"
#include "probfd/heuristics/cartesian/types.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/progress_report.h"
#include "probfd/value_type.h"

#include "algorithms/priority_queues.h"

#include <deque>
#include <memory>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace heuristics {
namespace cartesian {

/**
 * @brief Find abstract solutions using ILAO*.
 */
class AbstractSearch {
    CartesianCostFunction cost_function;
    CartesianHeuristic heuristic;
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<
        const AbstractState*,
        const ProbabilisticTransition*>>
        ptb;
    ProgressReport report;

public:
    AbstractSearch(
        Abstraction& abstraction,
        std::vector<value_t> operator_costs);

    std::unique_ptr<Solution> find_solution(
        Abstraction& abstraction,
        const AbstractState* init_id,
        utils::CountdownTimer& time_limit);

    void notify_split(int v);

    CartesianHeuristic& get_heuristic();
};

/**
 * @brief Calls topological value iteration to compute the complete optimal
 * value function (for states reachable from the initial state).
 */
std::vector<value_t> compute_distances(
    Abstraction& abstraction,
    CartesianHeuristic& heuristic,
    const std::vector<value_t>& costs);

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
