#include "probfd/pdbs/cegar/single_cegar.h"

#include "probfd/pdbs/cegar/flaw.h"
#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/policy_extraction.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/utils.h"

#include "probfd/multi_policy.h"
#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include "downward/plugins/plugin.h"

#include <cassert>
#include <functional>
#include <ostream>
#include <utility>

using namespace std;

namespace probfd::pdbs::cegar {

SingleCEGARResult::SingleCEGARResult(
    std::unique_ptr<ProjectionStateSpace>&& projection,
    std::unique_ptr<ProbabilityAwarePatternDatabase>&& pdb)
    : projection(std::move(projection))
    , pdb(std::move(pdb))
{
}

SingleCEGARResult::SingleCEGARResult(SingleCEGARResult&&) noexcept = default;
SingleCEGARResult&
SingleCEGARResult::operator=(SingleCEGARResult&&) noexcept = default;
SingleCEGARResult::~SingleCEGARResult() = default;

SingleCEGAR::SingleCEGAR(
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    std::shared_ptr<FlawFindingStrategy> flaw_strategy,
    bool wildcard,
    int max_pdb_size,
    double max_time,
    int var,
    std::unordered_set<int> blacklisted_variables)
    : rng_(std::move(rng))
    , flaw_strategy_(std::move(flaw_strategy))
    , wildcard_(wildcard)
    , max_pdb_size_(max_pdb_size)
    , max_time_(max_time)
    , var(var)
    , blacklisted_variables_(std::move(blacklisted_variables))
{
}

SingleCEGAR::~SingleCEGAR() = default;

bool SingleCEGAR::get_flaws(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    std::vector<Flaw>& flaws,
    value_t termination_cost,
    StateRank initial_state,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    std::unique_ptr<ProjectionMultiPolicy> policy =
        compute_optimal_projection_policy(
            *result.projection,
            result.pdb->get_value_table(),
            initial_state,
            *rng_,
            wildcard_);

    // abort here if no abstract solution could be found
    if (result.pdb->lookup_estimate(initial_state) == termination_cost) {
        log << "SingleCEGAR: Problem unsolvable" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSOLVABLE);
    }

    // find out if and why the abstract solution
    // would not work for the concrete task.
    // We always start with the initial state.
    const bool executable = flaw_strategy_->apply_policy(
        task_proxy,
        *result.projection,
        *result.pdb,
        *policy,
        blacklisted_variables_,
        flaws,
        timer);

    // Check for new flaws
    if (flaws.empty()) {
        // Check if policy is executable modulo blacklisting.
        // Even if there are no flaws, there might be goal violations
        // that did not make it into the flaw list.
        if (log.is_at_least_verbose()) {
            if (executable && blacklisted_variables_.empty()) {
                /*
                 * If there are no flaws, this does not guarantee that the
                 * plan is valid in the concrete state space because we might
                 * have ignored variables that have been blacklisted. Hence, the
                 * tests for empty blacklists.
                 */

                log << "SingleCEGAR: Task solved during "
                       "computation of "
                       "abstract policies."
                    << endl;
            } else {
                log << "SingleCEGAR: Flaw list empty."
                    << "No further refinements possible." << endl;
            }
        }

        return false;
    }

    return true;
}

void SingleCEGAR::refine(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const VariablesProxy& variables,
    const std::vector<Flaw>& flaws,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    assert(!flaws.empty());

    // pick a random flaw
    const Flaw& flaw = *rng_->choose(flaws);

    int flaw_var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: chosen flaw: pattern "
            << result.pdb->get_pattern();
    }

    if (log.is_at_least_verbose()) {
        log << " with a violated";
        if (flaw.is_precondition) {
            log << " precondition ";
        } else {
            log << " goal ";
        }
        log << "on " << flaw_var << endl;
    }

    // flaw_var is not yet in the collection
    // Note on precondition violations: flaw_var may be a goal variable but
    // nevertheless is added to the pattern causing the flaw and not to
    // a single new pattern.
    if (utils::is_product_within_limit(
            result.pdb->num_states(),
            variables[flaw_var].get_domain_size(),
            max_pdb_size_)) {
        if (log.is_at_least_verbose()) {
            log << "SingleCEGAR: add it to the pattern" << endl;
        }

        // compute new solution
        StateRankingFunction ranking_function(
            task_proxy.get_variables(),
            extended_pattern(result.pdb->get_pattern(), flaw_var));
        StateRank initial_state =
            ranking_function.get_abstract_rank(task_proxy.get_initial_state());

        result.projection = std::make_unique<ProjectionStateSpace>(
            task_proxy,
            task_cost_function,
            ranking_function,
            false,
            timer.get_remaining_time());

        result.pdb = std::make_unique<ProbabilityAwarePatternDatabase>(
            *result.projection,
            std::move(ranking_function),
            initial_state,
            *result.pdb,
            flaw_var,
            timer.get_remaining_time());

        return;
    }

    if (log.is_at_least_verbose()) {
        log << "could not add var due to size limits, blacklisting var" << endl;
    }

    blacklisted_variables_.insert(flaw_var);
}

SingleCEGARResult SingleCEGAR::generate_pdbs(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy_->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size_ << "\n"
            << "  max time: " << max_time_ << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard_ << "\n"
            << "  variable: " << var << "\n"
            << "  blacklisted variables: " << blacklisted_variables_ << endl;
    }

    if (log.is_at_least_normal()) {
        log << endl;
    }

    utils::CountdownTimer timer(max_time_);

    const VariablesProxy variables = task_proxy.get_variables();

    // Start with a solution of the trivial abstraction
    StateRankingFunction ranking_function(task_proxy.get_variables(), {var});
    StateRank initial_state =
        ranking_function.get_abstract_rank(task_proxy.get_initial_state());

    auto projection = std::make_unique<ProjectionStateSpace>(
        task_proxy,
        task_cost_function,
        ranking_function,
        false,
        timer.get_remaining_time());

    auto pdb = std::make_unique<ProbabilityAwarePatternDatabase>(
        *projection,
        std::move(ranking_function),
        initial_state,
        heuristics::ConstantEvaluator<StateRank>(0_vt),
        timer.get_remaining_time());

    SingleCEGARResult result(std::move(projection), std::move(pdb));

    if (log.is_at_least_normal()) {
        log << "SingleCEGAR initial collection: " << result.pdb->get_pattern();

        if (log.is_at_least_verbose()) {
            log << endl;
        }
    }

    std::vector<Flaw> flaws;

    // main loop of the algorithm
    int refinement_counter = 1;

    const value_t termination_cost =
        task_cost_function.get_non_goal_termination_cost();

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
            }

            if (!get_flaws(
                    result,
                    task_proxy,
                    flaws,
                    termination_cost,
                    initial_state,
                    timer,
                    log))
                break;

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                result,
                task_proxy,
                task_cost_function,
                variables,
                flaws,
                timer,
                log);

            ++refinement_counter;
            flaws.clear();

            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: current pattern: "
                    << result.pdb->get_pattern() << endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << "\nSingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter << endl;
    }

    return result;
}

} // namespace probfd::pdbs::cegar
