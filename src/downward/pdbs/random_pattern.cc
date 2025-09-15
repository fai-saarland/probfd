#include "downward/pdbs/random_pattern.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include <algorithm>
#include <unordered_set>

using namespace std;

namespace downward::pdbs {
static bool
time_limit_reached(const utils::CountdownTimer& timer, utils::LogProxy& log)
{
    if (timer.is_expired()) {
        if (log.is_at_least_normal()) {
            log << "Random pattern generation time limit reached" << endl;
        }
        return true;
    }
    return false;
}

Pattern generate_random_pattern(
    int max_pdb_size,
    utils::Duration max_time,
    utils::LogProxy& log,
    const shared_ptr<utils::RandomNumberGenerator>& rng,
    const VariableSpace& variables,
    int goal_variable,
    vector<vector<int>>& cg_neighbors)
{
    utils::CountdownTimer timer(max_time);
    int current_var = goal_variable;
    unordered_set<int> visited_vars;
    visited_vars.insert(current_var);

    int pdb_size = variables[current_var].get_domain_size();
    while (!time_limit_reached(timer, log)) {
        rng->shuffle(cg_neighbors[current_var]);

        /*
          Search for a neighbor which was not selected yet and which fits the
          size limit.
        */
        bool found_neighbor = false;
        for (int neighbor : cg_neighbors[current_var]) {
            int neighbor_dom_size = variables[neighbor].get_domain_size();
            if (!visited_vars.count(neighbor) && utils::is_product_within_limit(
                                                     pdb_size,
                                                     neighbor_dom_size,
                                                     max_pdb_size)) {
                pdb_size *= neighbor_dom_size;
                visited_vars.insert(neighbor);
                current_var = neighbor;
                found_neighbor = true;
                break;
            }
        }

        if (!found_neighbor) {
            break;
        }
    }

    Pattern pattern(visited_vars.begin(), visited_vars.end());
    sort(pattern.begin(), pattern.end());
    return pattern;
}

} // namespace pdbs
