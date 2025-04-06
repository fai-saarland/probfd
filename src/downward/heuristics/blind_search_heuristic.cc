#include "downward/heuristics/blind_search_heuristic.h"

#include "downward/task_utils/task_properties.h"
#include "downward/utils/logging.h"

#include <cstddef>
#include <limits>
#include <utility>

using namespace std;

namespace downward::blind_search_heuristic {
BlindSearchHeuristic::BlindSearchHeuristic(
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
    , min_operator_cost(task_properties::get_min_operator_cost(task_proxy))
{
    if (log.is_at_least_normal()) {
        log << "Initializing blind search heuristic..." << endl;
    }
}

int BlindSearchHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    if (task_properties::is_goal_state(task_proxy, state))
        return 0;
    else
        return min_operator_cost;
}

} // namespace blind_search_heuristic
