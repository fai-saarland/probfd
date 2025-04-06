#include "downward/heuristics/goal_count_heuristic.h"

#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::goal_count_heuristic {
GoalCountHeuristic::GoalCountHeuristic(
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(transform, cache_estimates, description, verbosity)
{
    if (log.is_at_least_normal()) {
        log << "Initializing goal count heuristic..." << endl;
    }
}

int GoalCountHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int unsatisfied_goal_count = 0;

    for (FactProxy goal : task_proxy.get_goals()) {
        const VariableProxy var = goal.get_variable();
        if (state[var] != goal) {
            ++unsatisfied_goal_count;
        }
    }
    return unsatisfied_goal_count;
}

} // namespace goal_count_heuristic
