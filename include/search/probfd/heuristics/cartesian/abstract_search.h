#ifndef PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_SEARCH_H
#define PROBFD_HEURISTICS_CARTESIAN_ABSTRACT_SEARCH_H

#include "probfd/heuristics/cartesian/probabilistic_transition.h"
#include "probfd/heuristics/cartesian/types.h"

#include "probfd/value_type.h"

#include "algorithms/priority_queues.h"

#include <deque>
#include <memory>
#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

/**
 * @brief Find abstract solutions using ILAO*.
 */
class AbstractSearch {
    const std::vector<value_t> operator_costs;

public:
    explicit AbstractSearch(std::vector<value_t> operator_costs);

    std::unique_ptr<Solution> find_solution(
        const std::deque<ProbabilisticTransition>& transitions,
        int init_id,
        const Goals& goal_ids);
    value_t get_h_value(int state_id) const;
    void copy_h_value_to_children(int v, int v1, int v2);

private:
    value_t set_h_value(int v, int state_id);
    void update_goal_distances(const Solution& solution);
};

/**
 * @brief Calls topological value iteration to compute the complete optimal
 * value function (for states reachable from the initial state).
 */
std::vector<value_t> compute_distances(
    const std::vector<ProbabilisticTransitions>& transitions,
    const std::vector<value_t>& costs);

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
