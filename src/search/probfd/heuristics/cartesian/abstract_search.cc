#include "probfd/heuristics/cartesian/abstract_search.h"

#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "cegar/abstract_state.h"

#include "utils/memory.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

AbstractSearch::AbstractSearch(vector<value_t> operator_costs)
    : operator_costs(std::move(operator_costs))
{
}

void AbstractSearch::update_goal_distances(const Solution&)
{
    /*
     * Idea: The values computed by ILAO* for every state can be used as
     * heuristic values, even if they are not part of the optimal solution.
     */

    // TODO: implement
    abort();
}

unique_ptr<Solution> AbstractSearch::find_solution(
    const deque<ProbabilisticTransition>&,
    int,
    const Goals&)
{
    // TODO: implement
    abort();
}

value_t AbstractSearch::get_h_value(int) const
{
    // TODO: implement
    abort();
}

value_t AbstractSearch::set_h_value(int, int)
{
    // TODO: implement
    abort();
}

void AbstractSearch::copy_h_value_to_children(int v, int v1, int v2)
{
    value_t h = get_h_value(v);

    // TODO: Grow value table by one here?

    set_h_value(v1, h);
    set_h_value(v2, h);
}

vector<value_t>
compute_distances(const deque<ProbabilisticTransition>&, const vector<value_t>&)
{
    /*
     * Simply call topological value iteration here to compute the full
     * value table.
     */

    // TODO: implement
    abort();
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd