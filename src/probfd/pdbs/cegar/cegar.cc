#include "probfd/pdbs/cegar/cegar.h"

#include "probfd/pdbs/cegar/flaw.h"
#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/utils.h"

#include "probfd/abstractions/policy_extraction.h"

#include "probfd/multi_policy.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"
#include "probfd/probabilistic_task.h"

#include <cassert>
#include <ostream>
#include <ranges>
#include <span>
#include <utility>

using namespace std;
using namespace std::ranges;

using namespace downward;

namespace probfd::pdbs::cegar {

/*
 * Implementation notes: The state space needs to be kept to find flaws in the
 * policy. Since it exists anyway, the algorithm is also a producer of
 * projection state spaces, not only of PDBs. Hence, the heap allocation to
 * return it to the user, in case he needs it, e.g. to compute the saturated
 * costs of the PDB.
 */
class CEGAR::PDBInfo {
    std::unique_ptr<ProbabilityAwarePatternDatabase> pdb;
    std::unique_ptr<ProjectionStateSpace> state_space;
    std::unique_ptr<ProjectionMultiPolicy> policy;

public:
    PDBInfo(
        SharedProbabilisticTask task,
        Pattern pattern,
        const State& initial_state,
        const heuristics::BlindEvaluator<StateRank>& h,
        value_t greedy_epsilon,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        const utils::CountdownTimer& timer);

    PDBInfo(
        SharedProbabilisticTask task,
        const ProbabilityAwarePatternDatabase& previous,
        int add_var,
        const State& initial_state,
        value_t greedy_epsilon,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        const utils::CountdownTimer& timer);

    PDBInfo(
        SharedProbabilisticTask task,
        const ProbabilityAwarePatternDatabase& merge_left,
        const ProbabilityAwarePatternDatabase& merge_right,
        const State& initial_state,
        value_t greedy_epsilon,
        utils::RandomNumberGenerator& rng,
        bool wildcard,
        const utils::CountdownTimer& timer);

    [[nodiscard]]
    const Pattern& get_pattern() const;

    [[nodiscard]]
    const ProjectionStateSpace& get_mdp() const;

    [[nodiscard]]
    const ProbabilityAwarePatternDatabase& get_pdb() const;

    [[nodiscard]]
    const ProjectionMultiPolicy& get_policy() const;

    [[nodiscard]]
    value_t get_policy_cost(const State& state) const;

    std::unique_ptr<ProjectionStateSpace> extract_state_space();
    std::unique_ptr<ProbabilityAwarePatternDatabase> extract_pdb();

    [[nodiscard]]
    bool is_goal(StateRank rank) const;

    void release();
};

CEGAR::PDBInfo::PDBInfo(
    SharedProbabilisticTask task,
    Pattern pattern,
    const State& initial_state,
    const heuristics::BlindEvaluator<StateRank>& h,
    value_t greedy_epsilon,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    const utils::CountdownTimer& timer)
    : pdb(std::make_unique<ProbabilityAwarePatternDatabase>(
          get_variables(task),
          std::move(pattern)))
    , state_space(
          std::make_unique<ProjectionStateSpace>(
              task,
              pdb->ranking_function,
              false,
              timer.get_remaining_time()))
{
    const StateRank abs_init = pdb->get_abstract_state(initial_state);

    compute_distances(
        *pdb,
        *state_space,
        abs_init,
        h,
        timer.get_remaining_time());

    policy = compute_optimal_projection_policy(
        *state_space,
        pdb->value_table,
        abs_init,
        greedy_epsilon,
        rng,
        wildcard);
}

CEGAR::PDBInfo::PDBInfo(
    SharedProbabilisticTask task,
    const ProbabilityAwarePatternDatabase& previous,
    int add_var,
    const State& initial_state,
    value_t greedy_epsilon,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    const utils::CountdownTimer& timer)
    : pdb(std::make_unique<ProbabilityAwarePatternDatabase>(
          get_variables(task),
          extended_pattern(previous.get_pattern(), add_var)))
    , state_space(
          std::make_unique<ProjectionStateSpace>(
              task,
              pdb->ranking_function,
              false,
              timer.get_remaining_time()))
{
    IncrementalPPDBEvaluator h(
        previous.value_table,
        pdb->ranking_function,
        add_var);

    const StateRank abs_init = pdb->get_abstract_state(initial_state);

    compute_distances(
        *pdb,
        *state_space,
        abs_init,
        h,
        timer.get_remaining_time());

    policy = compute_optimal_projection_policy(
        *state_space,
        pdb->value_table,
        abs_init,
        greedy_epsilon,
        rng,
        wildcard);
}

CEGAR::PDBInfo::PDBInfo(
    SharedProbabilisticTask task,
    const ProbabilityAwarePatternDatabase& left,
    const ProbabilityAwarePatternDatabase& right,
    const State& initial_state,
    value_t greedy_epsilon,
    utils::RandomNumberGenerator& rng,
    bool wildcard,
    const utils::CountdownTimer& timer)
    : pdb(std::make_unique<ProbabilityAwarePatternDatabase>(
          get_variables(task),
          utils::merge_sorted(left.get_pattern(), right.get_pattern())))
    , state_space(
          std::make_unique<ProjectionStateSpace>(
              task,
              pdb->ranking_function,
              false,
              timer.get_remaining_time()))
{
    const auto& term_costs = get_termination_costs(task);

    MergeEvaluator h(
        pdb->ranking_function,
        left,
        right,
        term_costs.get_non_goal_termination_cost());

    const StateRank abs_init = pdb->get_abstract_state(initial_state);

    compute_distances(
        *pdb,
        *state_space,
        abs_init,
        h,
        timer.get_remaining_time());

    policy = compute_optimal_projection_policy(
        *state_space,
        pdb->value_table,
        abs_init,
        greedy_epsilon,
        rng,
        wildcard);
}

const Pattern& CEGAR::PDBInfo::get_pattern() const
{
    return pdb->get_pattern();
}

const ProjectionStateSpace& CEGAR::PDBInfo::get_mdp() const
{
    assert(state_space);
    return *state_space;
}

const ProbabilityAwarePatternDatabase& CEGAR::PDBInfo::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

const ProjectionMultiPolicy& CEGAR::PDBInfo::get_policy() const
{
    return *policy;
}

value_t CEGAR::PDBInfo::get_policy_cost(const State& state) const
{
    return pdb->lookup_estimate(state);
}

std::unique_ptr<ProjectionStateSpace> CEGAR::PDBInfo::extract_state_space()
{
    return std::move(state_space);
}

std::unique_ptr<ProbabilityAwarePatternDatabase> CEGAR::PDBInfo::extract_pdb()
{
    return std::move(pdb);
}

bool CEGAR::PDBInfo::is_goal(StateRank rank) const
{
    return state_space->is_goal(rank);
}

void CEGAR::PDBInfo::release()
{
    state_space.reset();
    pdb.reset();
    policy.reset();
}

CEGAR::CEGAR(
    value_t convergence_epsilon,
    const shared_ptr<utils::RandomNumberGenerator>& arg_rng,
    std::shared_ptr<FlawFindingStrategy> flaw_strategy,
    bool wildcard,
    int arg_max_pdb_size,
    int arg_max_collection_size,
    std::vector<int> goals,
    std::unordered_set<int> blacklisted_variables)
    : convergence_epsilon(convergence_epsilon)
    , rng_(arg_rng)
    , flaw_strategy_(std::move(flaw_strategy))
    , wildcard_(wildcard)
    , max_pdb_size_(arg_max_pdb_size)
    , max_collection_size_(arg_max_collection_size)
    , goals_(std::move(goals))
    , blacklisted_variables_(std::move(blacklisted_variables))
{
}

CEGAR::~CEGAR() = default;

void CEGAR::print_collection(utils::LogProxy log) const
{
    log << "Unsolved patterns: [";

    auto unsolved_infos =
        std::ranges::subrange(pdb_infos_.begin(), unsolved_end);

    if (!unsolved_infos.empty()) {
        log << unsolved_infos.front().get_pattern();
        for (const auto& info : unsolved_infos | std::views::drop(1)) {
            log << ", " << info.get_pattern();
        }
    }

    log << "], Solved patterns: [";

    auto solved_infos = std::ranges::subrange(unsolved_end, solved_end);

    if (!solved_infos.empty()) {
        log << solved_infos.front().get_pattern();
        for (const auto& info : solved_infos | std::views::drop(1)) {
            log << ", " << info.get_pattern();
        }
    }

    log << "]" << endl;
}

void CEGAR::generate_trivial_solution_collection(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    assert(!goals_.empty());

    pdb_infos_.reserve(goals_.size());

    const auto& operators = get_operators(task);
    const auto& cost_function =
        get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    heuristics::BlindEvaluator<StateRank> h(
        operators,
        cost_function,
        term_costs);

    for (int var : goals_) {
        add_pattern_for_var(task, initial_state, h, var, timer);
    }

    unsolved_end = pdb_infos_.end();
    solved_end = pdb_infos_.end();

    if (log.is_at_least_normal()) {
        log << "CEGAR initial collection: ";
        print_collection(log);

        if (log.is_at_least_verbose()) { log << endl; }
    }
}

auto CEGAR::get_flaws(
    const ProbabilisticTaskTuple& task,
    const State& initial_state,
    std::vector<Flaw>& flaws,
    std::vector<int>& flaw_offsets,
    utils::CountdownTimer& timer,
    utils::LogProxy log) -> std::vector<PDBInfo>::iterator
{
    const auto& variables = get_variables(task);

    auto info_it = pdb_infos_.begin();
    auto flaw_offset_it = flaw_offsets.begin();

    while (info_it != unsolved_end) {
        auto& info = *info_it;

        auto accept_flaw = [&](const Flaw& flaw) {
            int var = flaw.variable;
            if (blacklisted_variables_.contains(var)) return false;

            const auto it = variable_to_info_.find(var);
            if ((it != variable_to_info_.end() &&
                 !can_merge_patterns(info_it, it->second)) ||
                !can_add_variable_to_pattern(variables, info_it, var)) {
                if (log.is_at_least_verbose()) {
                    log << "ignoring flaw on var " << var
                        << " due to size limits, blacklisting..." << endl;
                }

                blacklisted_variables_.insert(var);
                return false;
            }

            return true;
        };

        const size_t num_flaws_before = flaws.size();

        const bool executable = flaw_strategy_->apply_policy(
            task,
            initial_state,
            info.get_pdb().ranking_function,
            info.get_mdp(),
            info.get_policy(),
            flaws,
            accept_flaw,
            timer);

        const size_t new_num_flaws = flaws.size();

        // Check for new flaws
        if (new_num_flaws == num_flaws_before) {
            // Check if policy is executable modulo blacklisting.
            // Even if there are no flaws, there might be goal violations
            // that did not make it into the flaw list.
            if (executable && blacklisted_variables_.empty()) {
                /*
                 * If there are no flaws, this does not guarantee that the
                 * policy is valid in the concrete state space because we might
                 * have ignored variables that have been blacklisted. Hence, the
                 * tests for empty blacklists.
                 */
                flaws.clear();
                return info_it;
            }

            --unsolved_end;

            if (log.is_at_least_verbose()) {
                log << "CEGAR: Marking pattern as solved: "
                    << info_it->get_pattern() << std::endl;
                log << "CEGAR: Remaining unsolved patterns: "
                    << unsolved_end - pdb_infos_.begin() << std::endl;
            }

            if (info_it != unsolved_end) {
                auto& swapped_info = *unsolved_end;

                // update look-up table
                for (int var : swapped_info.get_pattern()) {
                    variable_to_info_[var] = info_it;
                }

                for (int var : info.get_pattern()) {
                    variable_to_info_[var] = unsolved_end;
                }

                std::swap(info, swapped_info);
            }

            continue;
        }

        *flaw_offset_it = static_cast<int>(new_num_flaws);

        ++info_it;
        ++flaw_offset_it;
    }

    return unsolved_end;
}

bool CEGAR::can_add_variable_to_pattern(
    const VariableSpace& variables,
    std::vector<PDBInfo>::iterator info_it,
    int var) const
{
    int pdb_size = info_it->get_pdb().num_states();
    int domain_size = variables[var].get_domain_size();

    const int limit = std::min(max_pdb_size_, remaining_size_ + pdb_size);
    return utils::is_product_within_limit(pdb_size, domain_size, limit);
}

bool CEGAR::can_merge_patterns(
    std::vector<PDBInfo>::iterator info_it1,
    std::vector<PDBInfo>::iterator info_it2) const
{
    int pdb_size1 = info_it1->get_pdb().num_states();
    int pdb_size2 = info_it2->get_pdb().num_states();

    const int limit =
        std::min(max_pdb_size_, remaining_size_ + pdb_size1 + pdb_size2);

    return utils::is_product_within_limit(pdb_size1, pdb_size2, limit);
}

void CEGAR::add_pattern_for_var(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    const heuristics::BlindEvaluator<StateRank>& h,
    int var,
    utils::CountdownTimer& timer)
{
    auto info_it = pdb_infos_.emplace(
        pdb_infos_.end(),
        task,
        Pattern{var},
        initial_state,
        h,
        convergence_epsilon,
        *rng_,
        wildcard_,
        timer);

    variable_to_info_[var] = info_it;
    remaining_size_ -= info_it->get_pdb().num_states();
}

void CEGAR::add_variable_to_pattern(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    std::vector<PDBInfo>::iterator info_it,
    int var,
    const utils::CountdownTimer& timer)
{
    PDBInfo& info = *info_it;

    const auto& pdb = info.get_pdb();

    // update collection size
    remaining_size_ += pdb.num_states();

    // compute new solution
    info = PDBInfo(
        task,
        pdb,
        var,
        initial_state,
        convergence_epsilon,
        *rng_,
        wildcard_,
        timer);

    // update collection size
    remaining_size_ -= info.get_pdb().num_states();

    // update look-up table
    variable_to_info_[var] = info_it;
}

void CEGAR::merge_patterns(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    std::vector<PDBInfo>::iterator info_it1,
    std::vector<PDBInfo>::iterator info_it2,
    const utils::CountdownTimer& timer)
{
    // Merge pattern at index2 into pattern at index2
    PDBInfo& solution1 = *info_it1;
    PDBInfo& solution2 = *info_it2;

    const ProbabilityAwarePatternDatabase& pdb1 = solution1.get_pdb();
    const ProbabilityAwarePatternDatabase& pdb2 = solution2.get_pdb();

    // update look-up table
    for (int var : solution2.get_pattern()) {
        variable_to_info_[var] = info_it1;
    }

    // store old pdb sizes
    int pdb_size1 = pdb1.num_states();
    int pdb_size2 = pdb2.num_states();

    // update collection size
    remaining_size_ += pdb_size1 + pdb_size2;

    // compute merge solution
    solution1 = PDBInfo(
        task,
        pdb1,
        pdb2,
        initial_state,
        convergence_epsilon,
        *rng_,
        wildcard_,
        timer);

    solution2.release();

    // update collection size
    remaining_size_ -= solution1.get_pdb().num_states();

    // fill gap if created
    if (info_it2 < unsolved_end && info_it2 != --unsolved_end) {
        auto& moved_from = *unsolved_end;

        // update look-up table
        for (int var : moved_from.get_pattern()) {
            variable_to_info_[var] = info_it2;
        }

        solution2 = std::move(moved_from);
        info_it2 = unsolved_end;
    }

    if (info_it2 != --solved_end) {
        auto& moved_from = *solved_end;

        // update look-up table
        for (int var : moved_from.get_pattern()) {
            variable_to_info_[var] = info_it2;
        }

        *info_it2 = std::move(moved_from);
    }
}

void CEGAR::refine(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    const std::vector<Flaw>& flaws,
    const std::vector<int>& flaw_offsets,
    const utils::CountdownTimer& timer,
    utils::LogProxy log)
{
    assert(!flaws.empty());

    // pick a random flaw
    int random_flaw_index = rng_->random(flaws.size());
    const Flaw& flaw = flaws[random_flaw_index];

    int solution_index = std::distance(
        flaw_offsets.begin(),
        std::ranges::upper_bound(flaw_offsets, random_flaw_index));
    auto solution_it = std::next(pdb_infos_.begin(), solution_index);
    int var = flaw.variable;

    if (log.is_at_least_verbose()) {
        log << "CEGAR: chosen flaw: pattern "
            << pdb_infos_[solution_index].get_pattern();
        log << " with a violated";
        if (flaw.is_precondition) {
            log << " precondition ";
        } else {
            log << " goal ";
        }
        log << "on " << var << endl;
    }

    const auto it = variable_to_info_.find(var);

    if (it != variable_to_info_.end()) {
        // var is already in another pattern of the collection
        auto other_it = it->second;
        assert(other_it != solution_it);
        assert(can_merge_patterns(solution_it, other_it));

        if (log.is_at_least_verbose()) {
            log << "CEGAR: var " << var << " is already in pattern "
                << other_it->get_pattern() << endl;
            log << "CEGAR: merge the two patterns" << endl;
        }

        merge_patterns(task, initial_state, solution_it, other_it, timer);
        return;
    }

    // var is not yet in the collection
    // Note on precondition violations: var may be a goal variable but
    // nevertheless is added to the pattern causing the flaw and not to
    // a single new pattern.
    if (log.is_at_least_verbose()) {
        log << "CEGAR: var " << var << " is not in the collection yet" << endl;
        log << "CEGAR: add it to the pattern" << endl;
    }

    add_variable_to_pattern(task, initial_state, solution_it, var, timer);
}

void CEGAR::generate_pdbs(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    ProjectionCollection& projections,
    PPDBCollection& pdbs,
    double max_time,
    utils::LogProxy log)
{
    if (log.is_at_least_normal()) {
        log << "CEGAR options: \n"
            << "  flaw strategy: " << flaw_strategy_->get_name() << "\n"
            << "  max pdb size: " << max_pdb_size_ << "\n"
            << "  max collection size: " << max_collection_size_ << "\n"
            << "  max time: " << max_time << "\n"
            << "  wildcard plans: " << std::boolalpha << wildcard_ << "\n"
            << "  goal variables: " << goals_ << "\n"
            << "  blacklisted variables: " << blacklisted_variables_ << endl;
    }

    if (log.is_at_least_normal()) { log << endl; }

    utils::CountdownTimer timer(max_time);

    // Start with a solution of the trivial abstraction
    generate_trivial_solution_collection(task, initial_state, timer, log);

    std::vector<Flaw> flaws;
    std::vector<int> flaw_offsets(pdb_infos_.size(), 0);
    std::vector<PDBInfo>::iterator solution_it;

    // main loop of the algorithm
    int refinement_counter = 1;

    try {
        for (;;) {
            if (log.is_at_least_verbose()) {
                log << "iteration #" << refinement_counter << endl;
            }

            solution_it = get_flaws(
                to_refs(task),
                initial_state,
                flaws,
                flaw_offsets,
                timer,
                log);

            if (flaws.empty()) {
                if (solution_it != unsolved_end) {
                    const auto& info = *solution_it;

                    assert(blacklisted_variables_.empty());

                    if (log.is_at_least_verbose()) {
                        const auto cost = info.get_policy_cost(initial_state);
                        log << "CEGAR: Task solved during computation of "
                               "abstract policies.\n"
                            << "CEGAR: Cost of policy: " << cost << endl;
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
            refine(task, initial_state, flaws, flaw_offsets, timer, log);

            ++refinement_counter;
            flaws.clear();

            if (log.is_at_least_verbose()) {
                log << "CEGAR: current collection size: "
                    << max_collection_size_ - remaining_size_ << endl;
                log << "CEGAR: current collection: ";
                print_collection(log);
            }

            if (log.is_at_least_verbose()) { log << endl; }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            log << "CEGAR: Time limit reached." << endl;
        }
    }

    if (log.is_at_least_normal()) { log << endl; }

    if (solution_it != unsolved_end) {
        auto& info = *solution_it;
        projections.emplace_back(info.extract_state_space());
        pdbs.emplace_back(info.extract_pdb());
    } else {
        for (auto& info :
             std::ranges::subrange(pdb_infos_.begin(), solved_end)) {
            projections.emplace_back(info.extract_state_space());
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
            << "  final collection summed PDB sizes: "
            << max_collection_size_ - remaining_size_ << endl;
    }
}

} // namespace probfd::pdbs::cegar
