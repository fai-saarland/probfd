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
namespace {

class SingleCEGAR {
    // Flaw finding strategy
    FlawFindingStrategy& flaw_strategy_;

    // behavior defining parameters
    const bool wildcard_;

    const int max_pdb_size_;
    std::unordered_set<int> blacklisted_variables_;

public:
    SingleCEGAR(
        cegar::FlawFindingStrategy& flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables = {});

    void run_cegar_loop(
        SingleCEGARResult& result,
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        utils::RandomNumberGenerator& rng,
        double max_time,
        utils::LogProxy log);

private:
    bool get_flaws(
        SingleCEGARResult& result,
        ProbabilisticTaskProxy task_proxy,
        std::vector<Flaw>& flaws,
        const State& initial_state,
        utils::RandomNumberGenerator& rng,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    void refine(
        SingleCEGARResult& result,
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const std::vector<Flaw>& flaws,
        utils::RandomNumberGenerator& rng,
        utils::CountdownTimer& timer,
        utils::LogProxy log);
};

SingleCEGAR::SingleCEGAR(
    FlawFindingStrategy& flaw_strategy,
    bool wildcard,
    int max_pdb_size,
    std::unordered_set<int> blacklisted_variables)
    : flaw_strategy_(flaw_strategy)
    , wildcard_(wildcard)
    , max_pdb_size_(max_pdb_size)
    , blacklisted_variables_(std::move(blacklisted_variables))
{
}

bool SingleCEGAR::get_flaws(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    std::vector<Flaw>& flaws,
    const State& initial_state,
    utils::RandomNumberGenerator& rng,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    StateRank init_state_rank = result.pdb->get_abstract_state(initial_state);

    std::unique_ptr<ProjectionMultiPolicy> policy =
        compute_optimal_projection_policy(
            *result.projection,
            result.pdb->get_value_table(),
            init_state_rank,
            rng,
            wildcard_);

    // abort here if no abstract solution could be found
    if (!result.projection->is_goal(init_state_rank) &&
        policy->get_decisions(init_state_rank).empty()) {
        log << "SingleCEGAR: Problem unsolvable" << endl;
        utils::exit_with(utils::ExitCode::SEARCH_UNSOLVABLE);
    }

    auto accept_flaw = [&](const Flaw& flaw) {
        int var = flaw.variable;
        if (blacklisted_variables_.contains(var)) return false;

        if (!utils::is_product_within_limit(
                result.pdb->num_states(),
                task_proxy.get_variables()[var].get_domain_size(),
                max_pdb_size_)) {
            if (log.is_at_least_verbose()) {
                log << "ignoring flaw on var " << var
                    << " due to size limit, blacklisting..." << endl;
            }

            blacklisted_variables_.insert(var);
            return false;
        }

        return true;
    };

    // find out if and why the abstract solution
    // would not work for the concrete task.
    // We always start with the initial state.
    const bool executable = flaw_strategy_.apply_policy(
        task_proxy,
        result.pdb->get_state_ranking_function(),
        *result.projection,
        *policy,
        flaws,
        accept_flaw,
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
    const std::vector<Flaw>& flaws,
    utils::RandomNumberGenerator& rng,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    assert(!flaws.empty());

    // pick a random flaw
    const Flaw& flaw = *rng.choose(flaws);

    int flaw_var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: chosen flaw: pattern "
            << result.pdb->get_pattern();
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
    assert(utils::is_product_within_limit(
        result.pdb->num_states(),
        task_proxy.get_variables()[flaw_var].get_domain_size(),
        max_pdb_size_));

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
        *result.pdb,
        flaw_var,
        initial_state,
        timer.get_remaining_time());
}

} // namespace

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

void SingleCEGAR::run_cegar_loop(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    utils::RandomNumberGenerator& rng,
    double max_time,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy_.get_name() << "\n"
            << "  max pdb size: " << max_pdb_size_ << "\n"
            << "  max time: " << max_time << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard_ << "\n"
            << "  blacklisted variables: " << blacklisted_variables_ << endl;
    }

    if (log.is_at_least_normal()) {
        log << endl;
    }

    utils::CountdownTimer timer(max_time);

    State initial_state = task_proxy.get_initial_state();

    if (log.is_at_least_normal()) {
        log << "SingleCEGAR initial collection: " << result.pdb->get_pattern();

        if (log.is_at_least_verbose()) {
            log << endl;
        }
    }

    std::vector<Flaw> flaws;

    // main loop of the algorithm
    int refinement_counter = 1;

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
            }

            if (!get_flaws(
                    result,
                    task_proxy,
                    flaws,
                    initial_state,
                    rng,
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
                flaws,
                rng,
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
}

void run_cegar_loop(
    SingleCEGARResult& result,
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    cegar::FlawFindingStrategy& flaw_strategy,
    std::unordered_set<int> blacklisted_variables,
    int max_pdb_size,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    double max_time,
    utils::LogProxy log)
{
    SingleCEGAR single_cegar(
        flaw_strategy,
        wildcard,
        max_pdb_size,
        std::move(blacklisted_variables));

    single_cegar.run_cegar_loop(
        result,
        task_proxy,
        task_cost_function,
        rng,
        max_time,
        std::move(log));
}

} // namespace probfd::pdbs::cegar
