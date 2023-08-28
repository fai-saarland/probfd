#include "probfd/heuristics/pdbs/cegar/flaw_generator.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"
#include "probfd/heuristics/pdbs/cegar/projection_info.h"

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/state_ranking_function.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/utils/guards.h"

#include "probfd/multi_policy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include "probfd/task_utils/task_properties.h"

#include <cassert>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

using namespace std;

FlawGenerator::FlawGenerator(
    std::shared_ptr<FlawFindingStrategy> flaw_strategy,
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool wildcard)
    : flaw_strategy(std::move(flaw_strategy))
    , rng(std::move(rng))
    , wildcard(wildcard)
{
}

std::optional<Flaw> FlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    ProjectionInfo& projection_info,
    const State& state,
    value_t termination_cost,
    std::unordered_set<int>& blacklist,
    int max_pdb_size,
    utils::LogProxy log,
    utils::CountdownTimer& timer)
{
    auto& [abstraction_mapping, projection_mdp, heuristic] = projection_info;

    const StateRank init_state = abstraction_mapping->get_abstract_rank(state);

    std::vector<value_t>& value_table = heuristic->get_value_table();

    pdbs::compute_value_table(
        *projection_mdp,
        *heuristic,
        init_state,
        timer.get_remaining_time(),
        value_table);

    if (value_table[init_state.id] == termination_cost) {
        log << "Problem unsolvable" << endl;
        return std::nullopt;
    }

    scope_exit scope([&] { flaws.clear(); });

    auto policy = pdbs::compute_optimal_projection_policy(
        *projection_mdp,
        init_state,
        value_table,
        *rng,
        wildcard);

    auto ignore_flaw = [&,
                        pdb_size = abstraction_mapping->num_states(),
                        variables = task_proxy.get_variables()](int var) {
        if (blacklist.contains(var)) return true;

        if (!utils::is_product_within_limit(
                pdb_size,
                variables[var].get_domain_size(),
                max_pdb_size)) {
            blacklist.insert(var);
            return true;
        };

        return false;
    };

    const bool executable = flaw_strategy->apply_policy(
        task_proxy,
        *projection_mdp,
        *abstraction_mapping,
        *policy,
        ignore_flaw,
        flaws,
        timer);

    /*
     * Even if there are no flaws, there might be goal violations
     * that did not make it into the flaw list.
     * If there are no flaws, this does not guarantee that the
     * plan is valid in the concrete state space because we might
     * have ignored variables that have been blacklisted. Hence the
     * empty blacklist check for the log output.
     */
    if (flaws.empty()) {
        if (log.is_at_least_verbose()) {
            if (executable && blacklist.empty()) {
                log << "Task solved during computation of "
                       "abstract policies.\n"
                       "Cost of policy: "
                    << value_table[init_state.id] << endl;

            } else {
                log << "Flaw list empty."
                    << "No further refinements possible." << endl;
            }
        }

        return std::nullopt;
    }

    return *rng->choose(flaws);
}

std::string FlawGenerator::get_name() const
{
    return flaw_strategy->get_name() +
           (wildcard ? " (regular)" : " (wildcard)");
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd