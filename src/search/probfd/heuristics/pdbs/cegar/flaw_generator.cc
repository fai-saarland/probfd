#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"
#include "probfd/heuristics/pdbs/cegar/policy_exploration_strategy.h"

#include "probfd/heuristics/pdbs/policy_extraction.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_info.h"

#include "probfd/multi_policy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

FlawGenerator::FlawGenerator(
    std::shared_ptr<PolicyExplorationStrategy> exploration_strategy,
    bool wildcard,
    int max_pdb_size,
    std::unordered_set<int> blacklisted_variables)
    : exploration_strategy(std::move(exploration_strategy))
    , wildcard(wildcard)
    , max_pdb_size(max_pdb_size)
    , blacklisted_variables(std::move(blacklisted_variables))
{
}

// Returns whether the policy is executable. Note that even if the policy is
// not executable, the output flaw list might be empty regardless, since
// blacklisted variables are never added.
bool FlawGenerator::generate_flaws(
    ProbabilisticTaskProxy task_proxy,
    ProjectionInfo& pdb_info,
    utils::RandomNumberGenerator& rng,
    std::vector<Flaw>& flaws,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    AbstractStateIndex initial_state =
        pdb_info.get_abstract_state(task_proxy.get_initial_state());

    auto policy = compute_optimal_projection_policy(
        pdb_info,
        initial_state,
        rng,
        wildcard);

    FlawFilter filter =
        [&, log = log, max = max_pdb_size, vars = task_proxy.get_variables()](
            int var_id) mutable {
            if (blacklisted_variables.contains(var_id)) return true;
            if (!utils::is_product_within_limit(
                    pdb_info.pdb->num_states(),
                    vars[var_id].get_domain_size(),
                    max)) {
                if (log.is_at_least_verbose()) {
                    log << "blacklisting flaw var " << var_id
                        << " due to size limits." << std::endl;
                }

                blacklisted_variables.insert(var_id);
                return true;
            }
            return false;
        };

    const bool guaranteed_flawless =
        exploration_strategy
            ->apply_policy(task_proxy, pdb_info, *policy, filter, flaws, timer);

    // Check if the projection was reported to be flawless.
    if (guaranteed_flawless) {
        if (log.is_at_least_verbose()) {
            log << "SingleCEGAR: Found flawless concrete policy with cost: "
                << pdb_info.lookup_estimate(initial_state) << std::endl;
        }
        return false;
    }

    // If there are no flaws, this does not guarantee that the
    // policy is valid in the concrete state space because we might
    // have ignored variables that have been blacklisted.
    return !flaws.empty();
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd