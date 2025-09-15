#include "downward/pdbs/pattern_collection_generator_multiple.h"

#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/utils.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "downward/fact_pair.h"

#include <vector>

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorMultiple::PatternCollectionGeneratorMultiple(
    int max_pdb_size,
    int max_collection_size,
    utils::Duration pattern_generation_max_time,
    utils::Duration total_max_time,
    utils::Duration stagnation_limit,
    double blacklist_trigger_percentage,
    bool enable_blacklist_on_stagnation,
    int random_seed,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_pdb_size(max_pdb_size)
    , pattern_generation_max_time(pattern_generation_max_time)
    , total_max_time(total_max_time)
    , stagnation_limit(stagnation_limit)
    , blacklisting_start_duration(total_max_time * blacklist_trigger_percentage)
    , enable_blacklist_on_stagnation(enable_blacklist_on_stagnation)
    , rng(utils::get_rng(random_seed))
    , random_seed(random_seed)
    , remaining_collection_size(max_collection_size)
    , blacklisting(false)
    , time_point_of_last_new_pattern(0.0)
{
}

void PatternCollectionGeneratorMultiple::check_blacklist_trigger_timer(
    const utils::CountdownTimer& timer)
{
    // Check if blacklisting should be started.
    if (!blacklisting &&
        timer.get_elapsed_time() > blacklisting_start_duration) {
        blacklisting = true;
        /*
          Also treat this time point as having seen a new pattern to avoid
          stopping due to stagnation right after enabling blacklisting.
        */
        time_point_of_last_new_pattern = timer.get_elapsed_time();
        if (log.is_at_least_normal()) {
            log << "given percentage of total time limit "
                << "exhausted; enabling blacklisting." << endl;
        }
    }
}

unordered_set<int>
PatternCollectionGeneratorMultiple::get_blacklisted_variables(
    vector<int>& non_goal_variables)
{
    unordered_set<int> blacklisted_variables;
    if (blacklisting && !non_goal_variables.empty()) {
        /*
          Randomize the number of non-goal variables for blacklisting.
          We want to choose at least 1 non-goal variable, so we pick a random
          value in the range [1, |non-goal variables|].
        */
        int blacklist_size = rng->random(non_goal_variables.size());
        ++blacklist_size;
        rng->shuffle(non_goal_variables);
        blacklisted_variables.insert(
            non_goal_variables.begin(),
            non_goal_variables.begin() + blacklist_size);
        if (log.is_at_least_debug()) {
            log << "blacklisting " << blacklist_size << " out of "
                << non_goal_variables.size() << " non-goal variables: ";
            for (int var : blacklisted_variables) { log << var << ", "; }
            log << endl;
        }
    }
    return blacklisted_variables;
}

void PatternCollectionGeneratorMultiple::handle_generated_pattern(
    PatternInformation&& pattern_info,
    set<Pattern>& generated_patterns,
    shared_ptr<PDBCollection>& generated_pdbs,
    const utils::CountdownTimer& timer)
{
    const Pattern& pattern = pattern_info.get_pattern();
    if (log.is_at_least_debug()) {
        log << "generated pattern " << pattern << endl;
    }
    if (generated_patterns.insert(std::move(pattern)).second) {
        /*
          compute_pattern generated a new pattern. Create/retrieve corresponding
          PDB, update collection size and reset time_point_of_last_new_pattern.
        */
        time_point_of_last_new_pattern = timer.get_elapsed_time();
        shared_ptr<PatternDatabase> pdb = pattern_info.get_pdb();
        remaining_collection_size -= pdb->get_size();
        generated_pdbs->push_back(std::move(pdb));
    }
}

bool PatternCollectionGeneratorMultiple::collection_size_limit_reached() const
{
    if (remaining_collection_size <= 0) {
        /*
          This value can become negative if the given size limits for
          pdb or collection size are so low that compute_pattern already
          violates the limit, possibly even with only using a single goal
          variable.
        */
        if (log.is_at_least_normal()) {
            log << "collection size limit reached" << endl;
        }
        return true;
    }
    return false;
}

bool PatternCollectionGeneratorMultiple::time_limit_reached(
    const utils::CountdownTimer& timer) const
{
    if (timer.is_expired()) {
        if (log.is_at_least_normal()) { log << "time limit reached" << endl; }
        return true;
    }
    return false;
}

bool PatternCollectionGeneratorMultiple::check_for_stagnation(
    const utils::CountdownTimer& timer)
{
    // Test if no new pattern was generated for longer than stagnation_limit.
    if (timer.get_elapsed_time() - time_point_of_last_new_pattern >
        stagnation_limit) {
        if (enable_blacklist_on_stagnation) {
            if (blacklisting) {
                if (log.is_at_least_normal()) {
                    log << "stagnation limit reached "
                        << "despite blacklisting, terminating" << endl;
                }
                return true;
            } else {
                if (log.is_at_least_normal()) {
                    log << "stagnation limit reached, "
                        << "enabling blacklisting" << endl;
                }
                blacklisting = true;
                time_point_of_last_new_pattern = timer.get_elapsed_time();
            }
        } else {
            if (log.is_at_least_normal()) {
                log << "stagnation limit reached, terminating" << endl;
            }
            return true;
        }
    }
    return false;
}

string PatternCollectionGeneratorMultiple::name() const
{
    return "multiple " + id() + " pattern collection generator";
}

PatternCollectionInformation
PatternCollectionGeneratorMultiple::compute_patterns(
    const SharedAbstractTask& task)
{
    if (log.is_at_least_normal()) {
        log << "max pdb size: " << max_pdb_size << endl;
        log << "max collection size: " << remaining_collection_size << endl;
        log << "max time: " << total_max_time << endl;
        log << "stagnation time limit: " << stagnation_limit << endl;
        log << "timer after which blacklisting is enabled: "
            << blacklisting_start_duration << endl;
        log << "enable blacklisting after stagnation: "
            << enable_blacklist_on_stagnation << endl;
    }

    utils::CountdownTimer timer(total_max_time);

    const auto& [variables, goals] =
        to_refs(slice_shared<VariableSpace, GoalFactList>(task));

    // Store the set of goals in random order.
    vector<FactPair> goal_facts = get_goals_in_random_order(goals, *rng);

    // Store the non-goal variables for potential blacklisting.
    vector<int> non_goal_variables = get_non_goal_variables(variables, goals);

    if (log.is_at_least_debug()) {
        log << "goal variables: ";
        for (FactPair goal : goal_facts) { log << goal.var << ", "; }
        log << endl;
        log << "non-goal variables: " << non_goal_variables << endl;
    }

    initialize(task);

    // Collect all unique patterns and their PDBs.
    set<Pattern> generated_patterns;
    shared_ptr<PDBCollection> generated_pdbs = make_shared<PDBCollection>();

    shared_ptr<utils::RandomNumberGenerator> pattern_computation_rng =
        make_shared<utils::RandomNumberGenerator>(random_seed);
    int num_iterations = 1;
    int goal_index = 0;
    while (true) {
        check_blacklist_trigger_timer(timer);

        unordered_set<int> blacklisted_variables =
            get_blacklisted_variables(non_goal_variables);

        int remaining_pdb_size = min(remaining_collection_size, max_pdb_size);
        utils::Duration remaining_time =
            min(timer.get_remaining_time(), pattern_generation_max_time);

        PatternInformation pattern_info = compute_pattern(
            remaining_pdb_size,
            remaining_time,
            pattern_computation_rng,
            task,
            goal_facts[goal_index],
            std::move(blacklisted_variables));
        handle_generated_pattern(
            std::move(pattern_info),
            generated_patterns,
            generated_pdbs,
            timer);

        if (collection_size_limit_reached() || time_limit_reached(timer) ||
            check_for_stagnation(timer)) {
            break;
        }

        ++num_iterations;
        ++goal_index;
        goal_index = goal_index % goal_facts.size();
        assert(utils::in_bounds(goal_index, goal_facts));
    }

    PatternCollectionInformation result =
        get_pattern_collection_info(to_refs(task), generated_pdbs, log);
    if (log.is_at_least_normal()) {
        log << name() << " number of iterations: " << num_iterations << endl;
        log << name() << " average time per generator: "
            << timer.get_elapsed_time() / num_iterations << endl;
    }
    return result;
}

} // namespace downward::pdbs
