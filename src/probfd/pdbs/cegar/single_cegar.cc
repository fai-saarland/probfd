#include "probfd/pdbs/cegar/single_cegar.h"

#include "probfd/pdbs/cegar/flaw.h"
#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/heuristics.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/projection_transformation.h"
#include "probfd/pdbs/utils.h"

#include "probfd/abstractions/distances.h"
#include "probfd/abstractions/policy_extraction.h"

#include "probfd/multi_policy.h"
#include "probfd/probabilistic_task.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"

#include <cassert>
#include <ostream>
#include <utility>

using namespace std;
using namespace downward;

namespace probfd::pdbs::cegar {
namespace {

class SingleCEGAR {
    // Convergence epsilon
    const value_t epsilon_;

    // Flaw finding strategy
    FlawFindingStrategy& flaw_strategy_;

    // behavior defining parameters
    const bool wildcard_;

    const int max_pdb_size_;
    std::unordered_set<int> blacklisted_variables_;

public:
    SingleCEGAR(
        value_t epsilon,
        cegar::FlawFindingStrategy& flaw_strategy,
        bool wildcard,
        int max_pdb_size,
        std::unordered_set<int> blacklisted_variables = {});

    void run_cegar_loop(
        ProjectionTransformation& transformation,
        const SharedProbabilisticTask& task,
        const State& initial_state,
        utils::RandomNumberGenerator& rng,
        utils::Duration max_time,
        utils::LogProxy log);

private:
    bool get_flaws(
        ProjectionTransformation& transformation,
        const SharedProbabilisticTask& task,
        std::vector<Flaw>& flaws,
        const State& initial_state,
        utils::RandomNumberGenerator& rng,
        utils::CountdownTimer& timer,
        utils::LogProxy log);

    void refine(
        ProjectionTransformation& transformation,
        const SharedProbabilisticTask& task,
        const State& initial_state,
        const std::vector<Flaw>& flaws,
        utils::RandomNumberGenerator& rng,
        utils::CountdownTimer& timer,
        utils::LogProxy log);
};

SingleCEGAR::SingleCEGAR(
    value_t epsilon,
    FlawFindingStrategy& flaw_strategy,
    bool wildcard,
    int max_pdb_size,
    std::unordered_set<int> blacklisted_variables)
    : epsilon_(epsilon)
    , flaw_strategy_(flaw_strategy)
    , wildcard_(wildcard)
    , max_pdb_size_(max_pdb_size)
    , blacklisted_variables_(std::move(blacklisted_variables))
{
}

bool SingleCEGAR::get_flaws(
    ProjectionTransformation& transformation,
    const SharedProbabilisticTask& task,
    std::vector<Flaw>& flaws,
    const State& initial_state,
    utils::RandomNumberGenerator& rng,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    auto& [pdb, projection] = transformation;

    const StateRank init_state_rank = pdb.get_abstract_state(initial_state);

    const auto policy = compute_optimal_projection_policy(
        *projection,
        pdb.value_table,
        init_state_rank,
        epsilon_,
        rng,
        wildcard_);

    // abort here if no abstract solution could be found
    if (!projection->is_goal(init_state_rank) &&
        policy->get_decisions(init_state_rank).empty()) {
        log.println("SingleCEGAR: Problem unsolvable");
        log.println("SingleCEGAR: Unsolvable pattern: {}", pdb.get_pattern());
        return false;
    }

    const auto& variables = get_variables(task);

    auto accept_flaw = [&](const Flaw& flaw) {
        int var = flaw.variable;
        if (blacklisted_variables_.contains(var)) return false;

        if (!utils::is_product_within_limit(
                pdb.num_states(),
                variables[var].get_domain_size(),
                max_pdb_size_)) {
            if (log.is_at_least_verbose()) {
                log.println(
                    "ignoring flaw on var {} due to size limit, "
                    "blacklisting...",
                    var);
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
        to_refs(task),
        initial_state,
        pdb.ranking_function,
        *projection,
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

                log.println(
                    "SingleCEGAR: Task solved during computation of "
                    "abstract policies.");
            } else {
                log.println(
                    "SingleCEGAR: Flaw list empty. No further refinements "
                    "possible.");
            }
        }

        return false;
    }

    return true;
}

void SingleCEGAR::refine(
    ProjectionTransformation& transformation,
    const SharedProbabilisticTask& task,
    const State& initial_state,
    const std::vector<Flaw>& flaws,
    utils::RandomNumberGenerator& rng,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    assert(!flaws.empty());

    auto& [pdb, projection] = transformation;

    // pick a random flaw
    const Flaw& flaw = *rng.choose(flaws);

    int flaw_var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log.println(
            "SingleCEGAR: chosen flaw: pattern {} with a violated {} on {}",
            pdb.get_pattern(),
            flaw.is_precondition ? "precondition" : "goal",
            flaw_var);
    }

    // flaw_var is not yet in the collection
    // Note on precondition violations: flaw_var may be a goal variable but
    // nevertheless is added to the pattern causing the flaw and not to
    // a single new pattern.
    assert(
        utils::is_product_within_limit(
            pdb.num_states(),
            get_variables(task)[flaw_var].get_domain_size(),
            max_pdb_size_));

    if (log.is_at_least_verbose()) {
        log.println("SingleCEGAR: add it to the pattern");
    }

    // compute new solution
    ProjectionTransformation new_transformation(
        task,
        extended_pattern(pdb.get_pattern(), flaw_var),
        false,
        timer.get_remaining_time());

    NonOwningIncrementalPPDBEvaluator h(
        pdb.value_table,
        new_transformation.pdb.ranking_function,
        flaw_var);

    compute_value_table(
        *new_transformation.projection,
        new_transformation.pdb.get_abstract_state(initial_state),
        h,
        new_transformation.pdb.value_table,
        timer.get_remaining_time(),
        epsilon_);

    transformation = std::move(new_transformation);
}

void SingleCEGAR::run_cegar_loop(
    ProjectionTransformation& transformation,
    const SharedProbabilisticTask& task,
    const State& initial_state,
    utils::RandomNumberGenerator& rng,
    utils::Duration max_time,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log.println(
            "SingleCEGAR options:\n"
            "  flaw strategy: {}\n"
            "  max pdb size: {}\n"
            "  max time: {}\n"
            "  wildcard plans: {}\n"
            "  blacklisted variables: {}",
            flaw_strategy_.get_name(),
            max_pdb_size_,
            max_time,
            wildcard_,
            blacklisted_variables_);
    }

    if (log.is_at_least_normal()) { log.println(); }

    utils::CountdownTimer timer(max_time);

    if (log.is_at_least_normal()) {
        log.println(
            "SingleCEGAR initial collection: {}",
            transformation.pdb.get_pattern());

        if (log.is_at_least_verbose()) { log.println(); }
    }

    std::vector<Flaw> flaws;

    // main loop of the algorithm
    int refinement_counter = 1;

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log.println("iteration #{}", refinement_counter);
            }

            if (!get_flaws(
                    transformation,
                    task,
                    flaws,
                    initial_state,
                    rng,
                    timer,
                    log))
                break;

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(transformation, task, initial_state, flaws, rng, timer, log);

            ++refinement_counter;
            flaws.clear();

            if (log.is_at_least_verbose()) {
                log.println(
                    "SingleCEGAR: current pattern: {}",
                    transformation.pdb.get_pattern());
            }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log.println("SingleCEGAR: Time limit reached.");
        }
    }

    if (log.is_at_least_normal()) {
        log.println(
            "\n"
            "SingleCEGAR statistics:\n"
            "  computation time: {}\n"
            "  number of iterations: {}",
            timer.get_elapsed_time(),
            refinement_counter);
    }
}

} // namespace

void run_cegar_loop(
    ProjectionTransformation& transformation,
    const SharedProbabilisticTask& task,
    const State& initial_state,
    value_t convergence_epsilon,
    cegar::FlawFindingStrategy& flaw_strategy,
    std::unordered_set<int> blacklisted_variables,
    int max_pdb_size,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    utils::Duration max_time,
    utils::LogProxy log)
{
    SingleCEGAR single_cegar(
        convergence_epsilon,
        flaw_strategy,
        wildcard,
        max_pdb_size,
        std::move(blacklisted_variables));

    single_cegar.run_cegar_loop(
        transformation,
        task,
        initial_state,
        rng,
        max_time,
        std::move(log));
}

} // namespace probfd::pdbs::cegar
