#include "probfd/heuristics/pdbs/cegar/cegar.h"

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/policy_extraction.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_info.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
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

#include "downward/plugins/plugin.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

CEGARResult::~CEGARResult() = default;

/*
 * Implementation notes: The state space needs to be kept to find flaws in the
 * policy. Since it exists anyway, the algorithm is also a producer of
 * projection state spaces, not only of PDBs. Hence the heap allocation to
 * return it to the user, in case he needs it, e.g. to compute the saturated
 * costs of the PDB.
 */
class CEGAR::PDBInfo {
    ProjectionInfo projection;
    std::unique_ptr<ProjectionMultiPolicy> policy;

public:
    PDBInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        utils::CountdownTimer& timer);

    PDBInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const ProbabilityAwarePatternDatabase& previous,
        int add_var,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        utils::CountdownTimer& timer);

    PDBInfo(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        const ProbabilityAwarePatternDatabase& merge_left,
        const ProbabilityAwarePatternDatabase& merge_right,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        utils::CountdownTimer& timer);

    const Pattern& get_pattern() const;

    const ProjectionInfo& get_projection() const;

    const ProbabilityAwarePatternDatabase& get_pdb() const;

    const ProjectionMultiPolicy& get_policy() const;

    unsigned int num_states() const;

    std::unique_ptr<ProjectionStateSpace> extract_state_space();
    std::unique_ptr<ProbabilityAwarePatternDatabase> extract_pdb();
};

CEGAR::PDBInfo::PDBInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    utils::CountdownTimer& timer)
    : projection(
          task_proxy,
          task_cost_function,
          std::move(pattern),
          task_proxy.get_initial_state(),
          heuristics::BlindEvaluator<AbstractStateIndex>(),
          false,
          timer.get_remaining_time())
    , policy(compute_optimal_projection_policy(
          projection,
          projection.get_abstract_state(task_proxy.get_initial_state()),
          rng,
          wildcard))
{
}

CEGAR::PDBInfo::PDBInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& previous,
    int add_var,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    utils::CountdownTimer& timer)
    : projection(
          task_proxy,
          task_cost_function,
          extended_pattern(previous.get_pattern(), add_var),
          task_proxy.get_initial_state(),
          previous,
          false,
          timer.get_remaining_time())
    , policy(compute_optimal_projection_policy(
          projection,
          projection.get_abstract_state(task_proxy.get_initial_state()),
          rng,
          wildcard))
{
}

CEGAR::PDBInfo::PDBInfo(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    utils::CountdownTimer& timer)
    : projection(
          task_proxy,
          task_cost_function,
          utils::merge_sorted(left.get_pattern(), right.get_pattern()),
          task_proxy.get_initial_state(),
          left,
          right,
          false,
          timer.get_remaining_time())
    , policy(compute_optimal_projection_policy(
          projection,
          projection.get_abstract_state(task_proxy.get_initial_state()),
          rng,
          wildcard))
{
}

const Pattern& CEGAR::PDBInfo::get_pattern() const
{
    return projection.get_pattern();
}

const ProjectionInfo& CEGAR::PDBInfo::get_projection() const
{
    return projection;
}

const ProbabilityAwarePatternDatabase& CEGAR::PDBInfo::get_pdb() const
{
    assert(projection.pdb);
    return *projection.pdb;
}

const ProjectionMultiPolicy& CEGAR::PDBInfo::get_policy() const
{
    return *policy;
}

unsigned int CEGAR::PDBInfo::num_states() const
{
    return projection.num_states();
}

std::unique_ptr<ProjectionStateSpace> CEGAR::PDBInfo::extract_state_space()
{
    return std::move(projection.mdp);
}

std::unique_ptr<ProbabilityAwarePatternDatabase> CEGAR::PDBInfo::extract_pdb()
{
    return std::move(projection.pdb);
}

CEGAR::CEGAR(
    utils::LogProxy log,
    const shared_ptr<utils::RandomNumberGenerator>& arg_rng,
    std::shared_ptr<FlawFindingStrategy> flaw_strategy,
    bool wildcard,
    int arg_max_pdb_size,
    int arg_max_collection_size,
    std::vector<int> goals,
    std::unordered_set<int> blacklisted_variables)
    : log(std::move(log))
    , rng(arg_rng)
    , flaw_strategy(flaw_strategy)
    , wildcard(wildcard)
    , max_pdb_size(arg_max_pdb_size)
    , max_collection_size(arg_max_collection_size)
    , goals(std::move(goals))
    , blacklisted_variables(std::move(blacklisted_variables))
{
}

CEGAR::~CEGAR() = default;

void CEGAR::print_collection() const
{
    assert(!pdb_infos.empty());
    log << "Marked unsolved: "
        << (std::ranges::subrange(pdb_infos.data(), unsolved_end) |
            std::views::transform(&CEGAR::PDBInfo::get_pattern))
        << ", Marked solved: "
        << (std::ranges::subrange(
                unsolved_end,
                pdb_infos.data() + pdb_infos.size()) |
            std::views::transform(&CEGAR::PDBInfo::get_pattern));
}

void CEGAR::generate_trivial_solution_collection(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    int& collection_size,
    utils::CountdownTimer& timer)
{
    assert(!goals.empty());

    pdb_infos.reserve(goals.size());

    for (int var : goals) {
        auto& info = pdb_infos.emplace_back(
            task_proxy,
            task_cost_function,
            Pattern{var},
            *rng,
            wildcard,
            timer);
        assert(!variable_to_info.contains(var));
        variable_to_info.emplace(var, std::ref(info));
        collection_size += info.num_states();
    }

    unsolved_end = pdb_infos.data() + pdb_infos.size();

    if (log.is_at_least_normal()) {
        log << "CEGAR initial collection: ";
        print_collection();

        if (log.is_at_least_verbose()) {
            log << endl;
        }
    }
}

CEGAR::PDBInfo* CEGAR::get_flaws(
    ProbabilisticTaskProxy task_proxy,
    std::vector<Flaw>& flaws,
    std::vector<int>& flaw_offsets,
    value_t termination_cost,
    utils::CountdownTimer& timer)
{
    auto* it = pdb_infos.data();
    auto* new_unsolved_end = it;

    // The following copies std::partition to enable a contiguous array of
    // unsolved, followed by a contiguous array of solved projections.
    while (it != unsolved_end) {
        PDBInfo& info = *it;

        // find out if and why the abstract solution
        // would not work for the concrete task.
        // We always start with the initial state.
        const size_t num_flaws_before = flaws.size();
        const bool guaranteed_flawless = flaw_strategy->apply_policy(
            task_proxy,
            info.get_projection(),
            info.get_policy(),
            blacklisted_variables,
            flaws,
            timer);

        const size_t new_num_flaws = flaws.size();

        // Check if the projection was reported to be flawless, in which case we
        // report it and end the refinement loop early. If there are no flaws,
        // this does not guarantee that the policy is valid in the concrete
        // state space because we might have ignored variables that have been
        // blacklisted.
        if (guaranteed_flawless) {
            assert(new_num_flaws == num_flaws_before);
            flaws.clear();
            return &info;
        }

        // Check if any flaws were generated.
        if (new_num_flaws != num_flaws_before) {
            if (new_unsolved_end != it) {
                for (int var : new_unsolved_end->get_pattern()) {
                    assert(variable_to_info.contains(var));
                    variable_to_info.find(var)->second = std::ref(*it);
                }
                for (int var : it->get_pattern()) {
                    assert(variable_to_info.contains(var));
                    variable_to_info.find(var)->second =
                        std::ref(*new_unsolved_end);
                }
                std::iter_swap(it, new_unsolved_end);
            }
            ++it;
            ++new_unsolved_end;
            flaw_offsets.emplace_back(static_cast<int>(new_num_flaws));
        } else {
            log << "Marking pattern " << info.get_pattern() << " as solved..."
                << endl;
            ++it;
        }
    }

    unsolved_end = new_unsolved_end;

    return nullptr;
}

bool CEGAR::can_add_variable(
    VariableProxy variable,
    const PDBInfo& info,
    int collection_size) const
{
    int pdb_size = info.num_states();
    int domain_size = variable.get_domain_size();

    if (!utils::is_product_within_limit(pdb_size, domain_size, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size * domain_size - pdb_size;
    return collection_size + added_size <= max_collection_size;
}

bool CEGAR::can_merge_patterns(
    const PDBInfo& left,
    const PDBInfo& right,
    int collection_size) const
{
    int pdb_size1 = left.num_states();
    int pdb_size2 = right.num_states();

    if (!utils::is_product_within_limit(pdb_size1, pdb_size2, max_pdb_size)) {
        return false;
    }

    int added_size = pdb_size1 * pdb_size2 - pdb_size1 - pdb_size2;
    return collection_size + added_size <= max_collection_size;
}

void CEGAR::add_variable_to_pattern(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    PDBInfo& info,
    int var,
    int& collection_size,
    utils::CountdownTimer& timer)
{
    // update collection size (remove old abstraction)
    collection_size -= info.num_states();

    // compute new solution
    info = PDBInfo(
        task_proxy,
        task_cost_function,
        info.get_pdb(),
        var,
        *rng,
        wildcard,
        timer);

    // update collection size (add new abstraction)
    collection_size += info.num_states();

    // update look-up table
    assert(!variable_to_info.contains(var));
    variable_to_info.emplace(var, std::ref(info));
}

void CEGAR::merge_patterns(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    PDBInfo& left,
    PDBInfo& right,
    int& collection_size,
    utils::CountdownTimer& timer)
{
    const ProbabilityAwarePatternDatabase& pdb1 = left.get_pdb();
    const ProbabilityAwarePatternDatabase& pdb2 = right.get_pdb();

    // update look-up table, abstractions will be merged into left storage
    for (int var : right.get_pattern()) {
        assert(variable_to_info.contains(var));
        variable_to_info.find(var)->second = std::ref(left);
    }

    // update collection size (remove previous abstractions)
    collection_size -= pdb1.num_states() + pdb2.num_states();

    // construct merged abstractions into left storage
    left = PDBInfo(
        task_proxy,
        task_cost_function,
        pdb1,
        pdb2,
        *rng,
        wildcard,
        timer);

    // update collection size (add merged abstraction)
    collection_size += left.num_states();

    // Now remove the right abstraction. If this produces a hole, fill it.
    if (&right < unsolved_end) {
        // An unsolved projection was merged. Move the last unsolved projection
        // into its place. Then move the last projection into the place of the
        // former last unsolved projection.
        auto& last_unsolved = *--unsolved_end;
        if (&right != &last_unsolved) {
            for (int var : last_unsolved.get_pattern()) {
                assert(variable_to_info.contains(var));
                variable_to_info.find(var)->second = std::ref(right);
            }
            right = std::move(last_unsolved);
        }
        if (&last_unsolved != &pdb_infos.back()) {
            for (int var : pdb_infos.back().get_pattern()) {
                assert(variable_to_info.contains(var));
                variable_to_info.find(var)->second = std::ref(last_unsolved);
            }
            last_unsolved = std::move(pdb_infos.back());
        }
    } else {
        // A solved projection was merged. Move the last projection
        // into its place.
        if (&right != &pdb_infos.back()) {
            for (int var : pdb_infos.back().get_pattern()) {
                assert(variable_to_info.contains(var));
                variable_to_info.find(var)->second = std::ref(right);
            }
            right = std::move(pdb_infos.back());
        }
    }

    pdb_infos.pop_back();
}

void CEGAR::refine(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const std::vector<Flaw>& flaws,
    const std::vector<int>& flaw_offsets,
    int& collection_size,
    utils::CountdownTimer& timer)
{
    assert(!flaws.empty());

    // pick a random flaw
    int random_flaw_index = rng->random(flaws.size());
    const Flaw& flaw = flaws[random_flaw_index];

    int flaw_index = std::distance(
        flaw_offsets.begin(),
        std::ranges::upper_bound(flaw_offsets, random_flaw_index));
    int var = flaw.variable;

    PDBInfo& flaw_info = pdb_infos[flaw_index];

    if (log.is_at_least_verbose()) {
        log << "CEGAR: chosen flaw: pattern " << flaw_info.get_pattern()
            << " with a violated";
        if (flaw.is_precondition) {
            log << " precondition ";
        } else {
            log << " goal ";
        }
        log << "on " << var << endl;
    }

    const auto it = variable_to_info.find(var);

    if (it != variable_to_info.end()) {
        // var is already in another pattern of the collection
        PDBInfo& other = it->second;
        assert(utils::contains(other.get_pattern(), var));
        assert(&other != &flaw_info);

        if (log.is_at_least_verbose()) {
            log << "CEGAR: var" << var << " is already in pattern "
                << other.get_pattern() << endl;
        }

        if (can_merge_patterns(flaw_info, other, collection_size)) {
            if (log.is_at_least_verbose()) {
                log << "CEGAR: merge the two patterns" << endl;
            }

            merge_patterns(
                task_proxy,
                task_cost_function,
                flaw_info,
                other,
                collection_size,
                timer);
            return;
        }
    } else {
        // var is not yet in the collection
        // Note on precondition violations: var may be a goal variable but
        // nevertheless is added to the pattern causing the flaw and not to
        // a single new pattern.
        if (log.is_at_least_verbose()) {
            log << "CEGAR: var" << var << " is not in the collection yet"
                << endl;
        }

        const VariableProxy variable = task_proxy.get_variables()[var];

        if (can_add_variable(variable, flaw_info, collection_size)) {
            if (log.is_at_least_verbose()) {
                log << "CEGAR: add it to the pattern" << endl;
            }

            add_variable_to_pattern(
                task_proxy,
                task_cost_function,
                flaw_info,
                var,
                collection_size,
                timer);
            return;
        }
    }

    if (log.is_at_least_verbose()) {
        log << "could not add var/merge pattern containing var "
            << "due to size limits, blacklisting var " << var << endl;
    }

    blacklisted_variables.insert(var);
}

CEGARResult CEGAR::generate_pdbs(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    utils::CountdownTimer& timer)
{
    if (log.is_at_least_normal()) {
        log << "CEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size << "\n"
            << "  max collection size: " << max_collection_size << "\n"
            << "  max time: " << timer.get_remaining_time() << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard << "\n"
            << "  goal variables: " << goals << "\n"
            << "  blacklisted variables: " << blacklisted_variables << endl;
    }

    if (log.is_at_least_normal()) {
        log << endl;
    }

    int collection_size = 0;

    // Start with a solution of the trivial abstraction
    generate_trivial_solution_collection(
        task_proxy,
        task_cost_function,
        collection_size,
        timer);

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    std::vector<Flaw> flaws;
    std::vector<int> flaw_offsets;
    flaw_offsets.reserve(pdb_infos.size());

    PDBInfo* solution = nullptr;

    // main loop of the algorithm
    int refinement_counter = 1;

    const value_t termination_cost =
        task_cost_function.get_non_goal_termination_cost();

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
                log << "CEGAR: current collection size: " << collection_size
                    << endl;
                log << "CEGAR: current collection: ";
                print_collection();
                log << endl;
            }

            solution = get_flaws(
                task_proxy,
                flaws,
                flaw_offsets,
                termination_cost,
                timer);

            if (flaws.empty()) {
                if (solution != nullptr) {
                    assert(blacklisted_variables.empty());

                    if (log.is_at_least_verbose()) {
                        log << "CEGAR: Task solved during computation of "
                               "abstract policies. Cost of policy: "
                            << solution->get_pdb().lookup_estimate(
                                   initial_state)
                            << endl;
                    }
                } else {
                    if (log.is_at_least_verbose()) {
                        log << "CEGAR: Flaw list empty. "
                            << "No further refinements possible." << endl;
                    }
                }

                break;
            }

            timer.throw_if_expired();

            // if there was a flaw, then refine the abstraction
            // such that said flaw does not occur again
            refine(
                task_proxy,
                task_cost_function,
                flaws,
                flaw_offsets,
                collection_size,
                timer);

            ++refinement_counter;
            flaws.clear();
            flaw_offsets.clear();
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "CEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << endl;
    }

    ProjectionCollection state_spaces;
    PPDBCollection pdbs;

    if (solution != nullptr) {
        state_spaces.emplace_back(solution->extract_state_space());
        pdbs.emplace_back(solution->extract_pdb());
    } else {
        for (auto& info : pdb_infos) {
            state_spaces.emplace_back(info.extract_state_space());
            pdbs.emplace_back(info.extract_pdb());
        }
    }

    if (log.is_at_least_normal()) {
        log << "CEGAR statistics:\n"
            << "  computation time: " << timer.get_elapsed_time() << "\n"
            << "  number of iterations: " << refinement_counter
            << "\n"
            // << "  final collection: " << *patterns << "\n"
            << "  final collection number of PDBs: " << pdbs.size() << "\n"
            << "  final collection summed PDB sizes: " << collection_size
            << endl;
    }

    return CEGARResult{std::move(state_spaces), std::move(pdbs)};
}

void add_cegar_wildcard_option_to_feature(plugins::Feature& feature)
{
    feature.add_option<bool>(
        "use_wildcard_policies",
        "if true, compute wildcard plans which are sequences of sets of "
        "operators that induce the same transition; otherwise compute regular "
        "plans which are sequences of single operators",
        "false");
}

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd