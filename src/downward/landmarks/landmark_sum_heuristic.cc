#include "downward/landmarks/landmark_sum_heuristic.h"

#include "downward/landmarks/landmark.h"
#include "downward/landmarks/landmark_factory.h"
#include "downward/landmarks/landmark_status_manager.h"
#include "downward/landmarks/util.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/task_utils/task_properties.h"

#include <limits>

using namespace std;

namespace downward::landmarks {
static bool are_dead_ends_reliable(
    const shared_ptr<LandmarkFactory>& lm_factory,
    const TaskProxy& task_proxy)
{
    if (task_properties::has_axioms(task_proxy)) {
        return false;
    }

    if (!lm_factory->supports_conditional_effects() &&
        task_properties::has_conditional_effects(task_proxy)) {
        return false;
    }

    return true;
}

LandmarkSumHeuristic::LandmarkSumHeuristic(
    const shared_ptr<LandmarkFactory>& lm_factory,
    bool pref,
    bool prog_goal,
    bool prog_gn,
    bool prog_r,
    const shared_ptr<AbstractTask>& transform,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : LandmarkHeuristic(
          pref,
          transform,
          cache_estimates,
          description,
          verbosity)
    , dead_ends_reliable(are_dead_ends_reliable(lm_factory, task_proxy))
{
    if (log.is_at_least_normal()) {
        log << "Initializing landmark sum heuristic..." << endl;
    }
    initialize(lm_factory, prog_goal, prog_gn, prog_r);
    compute_landmark_costs();
}

int LandmarkSumHeuristic::get_min_cost_of_achievers(
    const unordered_set<int>& achievers) const
{
    int min_cost = numeric_limits<int>::max();
    for (int id : achievers) {
        OperatorProxy op = get_operator_or_axiom(task_proxy, id);
        min_cost = min(min_cost, op.get_cost());
    }
    return min_cost;
}

void LandmarkSumHeuristic::compute_landmark_costs()
{
    /*
      This function runs under the assumption that landmark node IDs go
      from 0 to the number of landmarks - 1, therefore the entry in
      *min_first_achiever_costs* and *min_possible_achiever_costs*
      at index i corresponds to the entry for the landmark node with ID i.
    */

    /*
      For derived landmarks, we overapproximate that all operators are
      achievers. Since we do not want to explicitly store all operators
      in the achiever vector, we instead just compute the minimum cost
      over all operators and use this cost for all derived landmarks.
    */
    int min_operator_cost = task_properties::get_min_operator_cost(task_proxy);
    min_first_achiever_costs.reserve(lm_graph->get_num_landmarks());
    min_possible_achiever_costs.reserve(lm_graph->get_num_landmarks());
    for (auto& node : lm_graph->get_nodes()) {
        if (node->get_landmark().is_derived) {
            min_first_achiever_costs.push_back(min_operator_cost);
            min_possible_achiever_costs.push_back(min_operator_cost);
        } else {
            int min_first_achiever_cost =
                get_min_cost_of_achievers(node->get_landmark().first_achievers);
            min_first_achiever_costs.push_back(min_first_achiever_cost);
            int min_possible_achiever_cost = get_min_cost_of_achievers(
                node->get_landmark().possible_achievers);
            min_possible_achiever_costs.push_back(min_possible_achiever_cost);
        }
    }
}

int LandmarkSumHeuristic::get_heuristic_value(const State& ancestor_state)
{
    int h = 0;
    ConstBitsetView past =
        lm_status_manager->get_past_landmarks(ancestor_state);
    ConstBitsetView future =
        lm_status_manager->get_future_landmarks(ancestor_state);
    for (int id = 0; id < lm_graph->get_num_landmarks(); ++id) {
        if (future.test(id)) {
            int min_achiever_cost = past.test(id)
                                        ? min_possible_achiever_costs[id]
                                        : min_first_achiever_costs[id];
            if (min_achiever_cost < numeric_limits<int>::max()) {
                h += min_achiever_cost;
            } else {
                return DEAD_END;
            }
        }
    }
    return h;
}

bool LandmarkSumHeuristic::dead_ends_are_reliable() const
{
    return dead_ends_reliable;
}

} // namespace landmarks
