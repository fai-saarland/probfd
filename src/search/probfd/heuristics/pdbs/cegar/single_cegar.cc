#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/policy_extraction.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_info.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/multi_policy.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SingleCEGAR::SingleCEGAR(
    utils::LogProxy log,
    const shared_ptr<utils::RandomNumberGenerator>& arg_rng,
    std::shared_ptr<FlawFindingStrategy> flaw_strategy,
    bool wildcard,
    int arg_max_pdb_size,
    std::unordered_set<int> blacklisted_variables)
    : log(std::move(log))
    , rng(arg_rng)
    , flaw_strategy(flaw_strategy)
    , wildcard(wildcard)
    , max_pdb_size(arg_max_pdb_size)
    , blacklisted_variables(std::move(blacklisted_variables))
{
}

SingleCEGAR::~SingleCEGAR() = default;

void SingleCEGAR::run_refinement_loop(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    utils::CountdownTimer& timer)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size << "\n"
            << "  max time: " << timer.get_remaining_time() << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard << "\n"
            << "  blacklisted variables: " << blacklisted_variables << "\n"
            << endl;
    }

    int refinement_counter = 1;

    try {
        std::vector<Flaw> flaws;

        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: iteration #" << refinement_counter
                    << ", current pattern: " << pdb_info.get_pattern() << endl;
            }

            if (!get_flaws(
                    task_proxy,
                    task_cost_function,
                    pdb_info,
                    flaws,
                    timer)) {
                if (log.is_at_least_verbose()) {
                    log << "SingleCEGAR: Flaw list empty. "
                        << "No further refinements possible." << endl;
                }
                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(task_proxy, task_cost_function, pdb_info, flaws, timer);

            ++refinement_counter;
            flaws.clear();
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << "\nSingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter << "\n"
            << "  final pattern: " << pdb_info.get_pattern() << endl;
    }
}

bool SingleCEGAR::get_flaws(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    std::vector<Flaw>& flaws,
    utils::CountdownTimer& timer)
{
    AbstractStateIndex initial_state =
        pdb_info.get_abstract_state(task_proxy.get_initial_state());

    auto policy = compute_optimal_projection_policy(
        pdb_info,
        initial_state,
        *rng,
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
                        << " due to size limits." << endl;
                }

                blacklisted_variables.insert(var_id);
                return true;
            }
            return false;
        };

    const bool guaranteed_flawless =
        flaw_strategy
            ->apply_policy(task_proxy, pdb_info, *policy, filter, flaws, timer);

    // Check if the projection was reported to be flawless.
    if (guaranteed_flawless) {
        if (log.is_at_least_verbose()) {
            log << "SingleCEGAR: Found flawless concrete policy with cost: "
                << pdb_info.lookup_estimate(initial_state) << endl;
        }
        return false;
    }

    // If there are no flaws, this does not guarantee that the
    // policy is valid in the concrete state space because we might
    // have ignored variables that have been blacklisted.
    return !flaws.empty();
}

void SingleCEGAR::refine(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    const std::vector<Flaw>& flaws,
    utils::CountdownTimer& timer)
{
    assert(!flaws.empty());

    // pick a random flaw
    const Flaw& flaw = *rng->choose(flaws);
    int var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: found violated "
            << (flaw.is_precondition ? "precondition" : "goal") << " on " << var
            << endl;
    }

    // compute new solution
    pdb_info = ProjectionInfo(
        task_proxy,
        task_cost_function,
        extended_pattern(pdb_info.get_pattern(), var),
        task_proxy.get_initial_state(),
        *pdb_info.pdb,
        wildcard,
        timer.get_remaining_time());
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd