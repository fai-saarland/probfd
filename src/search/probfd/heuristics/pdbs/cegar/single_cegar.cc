#include "probfd/heuristics/pdbs/cegar/single_cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/multi_policy.h"
#include "probfd/task_proxy.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

SingleCEGARResult::~SingleCEGARResult() = default;

/*
 * Implementation notes: The state space needs to be kept to find flaws in the
 * policy. Since it exists anyway, the algorithm is also a producer of
 * projection state spaces, not only of PDBs. Hence the heap allocation to
 * return it to the user, in case he needs it, e.g. to compute the saturated
 * costs of the PDB.
 */
class SingleCEGAR::PDBInfo {
    std::unique_ptr<ProjectionStateSpace> state_space;
    StateRank initial_state;
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;
    std::unique_ptr<ProjectionMultiPolicy> policy;

public:
    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        FDRSimpleCostFunction& task_cost_function,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        utils::CountdownTimer& timer);

    PDBInfo(
        const ProbabilisticTaskProxy& task_proxy,
        StateRankingFunction ranking_function,
        FDRSimpleCostFunction& task_cost_function,
        utils::RandomNumberGenerator& rng,
        const ProbabilityAwarePatternDatabase& previous,
        int add_var,
        bool wildcard,
        utils::CountdownTimer& timer);

    const Pattern& get_pattern() const;

    const ProjectionStateSpace& get_mdp() const;

    const ProbabilityAwarePatternDatabase& get_pdb() const;

    const ProjectionMultiPolicy& get_policy() const;

    value_t get_policy_cost(const State& state) const;

    std::unique_ptr<ProjectionStateSpace> extract_state_space();
    std::unique_ptr<ProbabilityAwarePatternDatabase> extract_pdb();

    bool solution_exists(value_t termination_cost) const;

    bool is_goal(StateRank rank) const;
};

SingleCEGAR::PDBInfo::PDBInfo(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction ranking_function,
    FDRSimpleCostFunction& task_cost_function,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    utils::CountdownTimer& timer)
    : state_space(new ProjectionStateSpace(
          task_proxy,
          ranking_function,
          task_cost_function,
          false,
          timer.get_remaining_time()))
    , initial_state(
          ranking_function.get_abstract_rank(task_proxy.get_initial_state()))
    , pdb(new ProbabilityAwarePatternDatabase(
          *state_space,
          std::move(ranking_function),
          initial_state,
          heuristics::ConstantEvaluator<StateRank>(0_vt),
          timer.get_remaining_time()))
    , policy(pdb->compute_optimal_projection_policy(
          *state_space,
          initial_state,
          rng,
          wildcard))
{
}

SingleCEGAR::PDBInfo::PDBInfo(
    const ProbabilisticTaskProxy& task_proxy,
    StateRankingFunction ranking_function,
    FDRSimpleCostFunction& task_cost_function,
    utils::RandomNumberGenerator& rng,
    const ProbabilityAwarePatternDatabase& previous,
    int add_var,
    bool wildcard,
    utils::CountdownTimer& timer)
    : state_space(new ProjectionStateSpace(
          task_proxy,
          ranking_function,
          task_cost_function,
          false,
          timer.get_remaining_time()))
    , initial_state(
          ranking_function.get_abstract_rank(task_proxy.get_initial_state()))
    , pdb(new ProbabilityAwarePatternDatabase(
          *state_space,
          std::move(ranking_function),
          initial_state,
          previous,
          add_var,
          timer.get_remaining_time()))
    , policy(pdb->compute_optimal_projection_policy(
          *state_space,
          initial_state,
          rng,
          wildcard))
{
}

const Pattern& SingleCEGAR::PDBInfo::get_pattern() const
{
    return pdb->get_pattern();
}

const ProjectionStateSpace& SingleCEGAR::PDBInfo::get_mdp() const
{
    assert(state_space);
    return *state_space;
}

const ProbabilityAwarePatternDatabase& SingleCEGAR::PDBInfo::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

const ProjectionMultiPolicy& SingleCEGAR::PDBInfo::get_policy() const
{
    return *policy;
}

value_t SingleCEGAR::PDBInfo::get_policy_cost(const State& state) const
{
    return pdb->lookup_estimate(state);
}

std::unique_ptr<ProjectionStateSpace>
SingleCEGAR::PDBInfo::extract_state_space()
{
    return std::move(state_space);
}

std::unique_ptr<ProbabilityAwarePatternDatabase>
SingleCEGAR::PDBInfo::extract_pdb()
{
    return std::move(pdb);
}

bool SingleCEGAR::PDBInfo::solution_exists(value_t termination_cost) const
{
    return pdb->lookup_estimate(initial_state) != termination_cost;
}

bool SingleCEGAR::PDBInfo::is_goal(StateRank rank) const
{
    return state_space->is_goal(rank);
}

SingleCEGAR::SingleCEGAR(
    int max_pdb_size,
    double max_time,
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy,
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    bool wildcard,
    int goal,
    utils::LogProxy log,
    std::unordered_set<int> blacklisted_variables)
    : max_pdb_size(max_pdb_size)
    , max_time(max_time)
    , flaw_strategy(std::move(flaw_strategy))
    , rng(std::move(rng))
    , wildcard(wildcard)
    , goal(goal)
    , log(std::move(log))
    , blacklisted_variables(std::move(blacklisted_variables))
{
}

SingleCEGAR::~SingleCEGAR() = default;

bool SingleCEGAR::get_flaws(
    const PDBInfo& pdb_info,
    const ProbabilisticTaskProxy& task_proxy,
    std::vector<Flaw>& flaws,
    value_t termination_cost,
    utils::CountdownTimer& timer)
{
    if (!pdb_info.solution_exists(termination_cost)) {
        log << "SingleCEGAR: Problem unsolvable" << endl;
        return true;
    }

    const bool executable = flaw_strategy->apply_policy(
        task_proxy,
        pdb_info.get_mdp(),
        pdb_info.get_pdb(),
        pdb_info.get_policy(),
        blacklisted_variables,
        flaws,
        timer);

    /*
     * Even if there are no flaws, there might be goal violations
     * that did not make it into the flaw list.
     * If there are no flaws, this does not guarantee that the
     * plan is valid in the concrete state space because we might
     * have ignored variables that have been blacklisted. Hence the
     * tests for empty blacklists.
     */
    return flaws.empty() && executable && blacklisted_variables.empty();
}

bool SingleCEGAR::can_add_variable(
    const PDBInfo& pdb_info,
    const VariablesProxy& variables,
    int var) const
{
    const int pdb_size = pdb_info.get_pdb().num_states();
    const int domain_size = variables[var].get_domain_size();

    return utils::is_product_within_limit(pdb_size, domain_size, max_pdb_size);
}

void SingleCEGAR::add_variable_to_pattern(
    PDBInfo& pdb_info,
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    int var,
    utils::CountdownTimer& timer)
{
    auto& pdb = pdb_info.get_pdb();

    pdb_info = PDBInfo(
        task_proxy,
        StateRankingFunction(
            task_proxy.get_variables(),
            extended_pattern(pdb.get_pattern(), var)),
        task_cost_function,
        *rng,
        pdb,
        var,
        wildcard,
        timer);
}

void SingleCEGAR::refine(
    PDBInfo& pdb_info,
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const VariablesProxy& variables,
    const std::vector<Flaw>& flaws,
    utils::CountdownTimer& timer)
{
    assert(!flaws.empty());

    // pick a random flaw
    const Flaw& flaw = *rng->choose(flaws);
    int var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "SingleCEGAR: chosen flaw: violated";
        if (flaw.is_precondition) {
            log << " precondition ";
        } else {
            log << " goal ";
        }
        log << "on " << var << endl;
    }

    if (can_add_variable(pdb_info, variables, var)) {
        if (log.is_at_least_verbose()) {
            log << "SingleCEGAR: add it to the pattern" << endl;
        }

        add_variable_to_pattern(
            pdb_info,
            task_proxy,
            task_cost_function,
            var,
            timer);
        return;
    }

    if (log.is_at_least_verbose()) {
        log << "could not add var/merge pattern containing var "
            << "due to size limits, blacklisting var" << endl;
    }

    blacklisted_variables.insert(var);
}

SingleCEGARResult SingleCEGAR::generate_pdb(
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function)
{
    if (log.is_at_least_normal()) {
        log << "SingleCEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size << "\n"
            << "  max time: " << max_time << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard
            << std::noboolalpha << "\n"
            << "  goal variable: " << goal << "\n"
            << "  blacklisted variables: " << blacklisted_variables << "\n"
            << endl;
    }

    utils::CountdownTimer timer(max_time);

    const VariablesProxy variables = task_proxy.get_variables();

    // Start with a solution of the trivial abstraction
    std::unique_ptr<PDBInfo> pdb_info(new PDBInfo(
        task_proxy,
        StateRankingFunction(task_proxy.get_variables(), {goal}),
        task_cost_function,
        *rng,
        wildcard,
        timer));

    if (log.is_at_least_normal()) {
        log << "SingleCEGAR initial pattern: " << pdb_info->get_pattern();

        if (log.is_at_least_verbose()) {
            log << endl;
        }
    }

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    std::vector<Flaw> flaws;

    // main loop of the algorithm
    int refinement_counter = 1;

    const value_t termination_cost =
        task_cost_function.get_non_goal_termination_cost();

    try {
        for (;; ++refinement_counter, flaws.clear()) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
            }

            bool solution_found = get_flaws(
                *pdb_info,
                task_proxy,
                flaws,
                termination_cost,
                timer);

            if (solution_found) {
                assert(blacklisted_variables.empty());

                if (log.is_at_least_verbose()) {
                    log << "SingleCEGAR: Task solved during computation of "
                           "abstract policies.\n"
                           "SingleCEGAR: Cost of policy: "
                        << pdb_info->get_policy_cost(initial_state) << endl;
                }
                break;
            }

            if (flaws.empty()) {
                if (log.is_at_least_verbose()) {
                    log << "SingleCEGAR: Flaw list empty."
                        << "No further refinements possible." << endl;
                }

                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                *pdb_info,
                task_proxy,
                task_cost_function,
                variables,
                flaws,
                timer);

            if (log.is_at_least_verbose()) {
                log << "SingleCEGAR: current pattern: "
                    << pdb_info->get_pattern() << endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "SingleCEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << endl
            << "SingleCEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter << "\n"
            << "  final collection: " << pdb_info->get_pattern() << "\n";
    }

    return SingleCEGARResult{
        pdb_info->extract_state_space(),
        pdb_info->extract_pdb()};
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd