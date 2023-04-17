#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "utils/rng.h"

#include <cassert>
#include <limits>

namespace probfd {
namespace heuristics {
namespace pdbs {

Pattern extended_pattern(const Pattern& pattern, int add_var)
{
    assert(!utils::contains(pattern, add_var));

    Pattern added;
    added.reserve(pattern.size() + 1);
    auto it = std::upper_bound(pattern.begin(), pattern.end(), add_var);
    added.insert(added.end(), pattern.begin(), it);
    added.emplace_back(add_var);
    added.insert(added.end(), it, pattern.end());

    return added;
}

std::vector<int> get_goals_in_random_order(
    const ProbabilisticTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng)
{
    std::vector<int> goals;
    
    for (const auto fact : task_proxy.get_goals()) {
        goals.push_back(fact.get_variable().get_id());
    }

    rng.shuffle(goals);

    return goals;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
