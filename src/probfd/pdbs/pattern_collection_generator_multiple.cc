#include "probfd/pdbs/pattern_collection_generator_multiple.h"

#include "probfd/pdbs/fully_additive_finder.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/pdbs/saturation.h"
#include "probfd/pdbs/trivial_finder.h"

#include "probfd/tasks/range_operator_cost_function.h"

#include "probfd/cost_function.h"
#include "probfd/probabilistic_task.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"
#include "probfd/probabilistic_operator_space.h"

#include <cassert>
#include <ostream>
#include <set>
#include <span>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::pdbs {

namespace {
vector<FactPair> get_goals_in_random_order(
    const GoalFactList& goals,
    utils::RandomNumberGenerator& rng)
{
    vector<FactPair> goal_facts =
        downward::task_properties::get_fact_pairs(goals);
    rng.shuffle(goal_facts);
    return goal_facts;
}

vector<int> get_non_goal_variables(
    const VariableSpace& variables,
    const GoalFactList& goals)
{
    size_t num_vars = variables.size();
    vector<bool> is_goal(num_vars, false);
    for (FactPair goal : goals) { is_goal[goal.var] = true; }

    vector<int> non_goal_variables;
    non_goal_variables.reserve(num_vars - goals.size());
    for (int var_id = 0; var_id < static_cast<int>(num_vars); ++var_id) {
        if (!is_goal[var_id]) { non_goal_variables.push_back(var_id); }
    }
    return non_goal_variables;
}

} // namespace

PatternCollectionGeneratorMultiple::PatternCollectionGeneratorMultiple(
    int max_pdb_size,
    int max_collection_size,
    double pattern_generation_max_time,
    double total_max_time,
    double stagnation_limit,
    double blacklist_trigger_percentage,
    bool enable_blacklist_on_stagnation,
    bool use_saturated_costs,
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    std::string implementation_name,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , max_pdb_size_(max_pdb_size)
    , max_collection_size_(max_collection_size)
    , pattern_generation_max_time_(pattern_generation_max_time)
    , total_max_time_(total_max_time)
    , stagnation_limit_(stagnation_limit)
    , blacklisting_start_time_(total_max_time_ * blacklist_trigger_percentage)
    , enable_blacklist_on_stagnation_(enable_blacklist_on_stagnation)
    , use_saturated_costs_(use_saturated_costs)
    , rng_(std::move(rng))
    , implementation_name_(std::move(implementation_name))
{
}

bool PatternCollectionGeneratorMultiple::collection_size_limit_reached(
    int remaining_collection_size) const
{
    if (remaining_collection_size <= 0) {
        /*
          This value can become negative if the given size limits for
          pdb or collection size are so low that compute_pattern already
          violates the limit, possibly even with only using a single goal
          variable.
        */
        if (log_.is_at_least_normal()) {
            log_ << "collection size limit reached" << endl;
        }
        return true;
    }
    return false;
}

bool PatternCollectionGeneratorMultiple::time_limit_reached(
    const utils::CountdownTimer& timer) const
{
    if (timer.is_expired()) {
        if (log_.is_at_least_normal()) { log_ << "time limit reached" << endl; }
        return true;
    }
    return false;
}

PatternCollectionInformation PatternCollectionGeneratorMultiple::generate(
    const SharedProbabilisticTask& task)
{
    if (log_.is_at_least_normal()) {
        log_ << "max pdb size: " << max_pdb_size_ << endl;
        log_ << "max collection size: " << max_collection_size_ << endl;
        log_ << "max time: " << total_max_time_ << endl;
        log_ << "stagnation time limit: " << stagnation_limit_ << endl;
        log_ << "timer after which blacklisting is enabled: "
             << blacklisting_start_time_ << endl;
        log_ << "enable blacklisting after stagnation: "
             << enable_blacklist_on_stagnation_ << endl;
    }

    const auto& variables = get_variables(task);
    const auto& operators = get_operators(task);
    const auto& goals = get_goal(task);
    const auto& cost_function = get_cost_function(task);

    std::vector<value_t> costs(operators.get_num_operators());

    for (const auto op : operators) {
        costs[op.get_id()] = cost_function.get_operator_cost(op.get_id());
    }

    utils::CountdownTimer timer(total_max_time_);

    // Store the set of goals in random order.
    vector<FactPair> goal_facts = get_goals_in_random_order(goals, *rng_);

    // Store the non-goal variables for potential blacklisting.
    vector<int> non_goal_variables = get_non_goal_variables(variables, goals);

    if (log_.is_at_least_debug()) {
        log_ << "goal variables: ";
        for (FactPair goal : goals) { log_ << goal.var << ", "; }
        log_ << endl;
        log_ << "non-goal variables: " << non_goal_variables << endl;
    }

    // Collect all unique patterns and their PDBs.
    set<Pattern> generated_patterns;
    PPDBCollection generated_pdbs;

    int num_iterations = 0;
    int goal_index = 0;
    bool blacklisting = false;
    double time_point_of_last_new_pattern = 0.0;
    int remaining_collection_size = max_collection_size_;

    auto adapted_cost_function =
        downward::extra_tasks::make_shared_range_cf(std::move(costs));

    auto adapted = replace(task, adapted_cost_function);

    std::vector<value_t> saturated_costs(operators.get_num_operators());

    while (true) {
        // Check if blacklisting should be started.
        if (!blacklisting &&
            timer.get_elapsed_time() > blacklisting_start_time_) {
            blacklisting = true;
            /*
              Also treat this time point as having seen a new pattern to avoid
              stopping due to stagnation right after enabling blacklisting.
            */
            time_point_of_last_new_pattern = timer.get_elapsed_time();
            if (log_.is_at_least_normal()) {
                log_ << "given percentage of total time limit "
                     << "exhausted; enabling blacklisting." << endl;
            }
        }

        // Get blacklisted variables
        unordered_set<int> blacklisted_variables;
        if (blacklisting && !non_goal_variables.empty()) {
            /*
              Randomize the number of non-goal variables for blacklisting.
              We want to choose at least 1 non-goal variable, so we pick a
              random value in the range [1, |non-goal variables|].
            */
            int blacklist_size = rng_->random(non_goal_variables.size());
            ++blacklist_size;
            rng_->shuffle(non_goal_variables);
            blacklisted_variables.insert(
                non_goal_variables.begin(),
                non_goal_variables.begin() + blacklist_size);
            if (log_.is_at_least_debug()) {
                log_ << "blacklisting " << blacklist_size << " out of "
                     << non_goal_variables.size() << " non-goal variables: ";
                for (int var : blacklisted_variables) { log_ << var << ", "; }
                log_ << endl;
            }
        }

        int remaining_pdb_size = min(remaining_collection_size, max_pdb_size_);
        double remaining_time =
            min(static_cast<double>(timer.get_remaining_time()),
                pattern_generation_max_time_);

        auto [pdb, state_space] = compute_pattern(
            remaining_pdb_size,
            remaining_time,
            rng_,
            adapted,
            goals[goal_index],
            std::move(blacklisted_variables));

        const Pattern& pattern = pdb.get_pattern();
        if (log_.is_at_least_debug()) {
            log_ << "generated PDB with pattern " << pattern << endl;
        }

        if (generated_patterns.insert(pattern).second) {
            if (use_saturated_costs_) {
                compute_saturated_costs(
                    *state_space,
                    pdb.value_table,
                    saturated_costs);

                for (auto&& [cost, dec] :
                     std::views::zip(*adapted_cost_function, saturated_costs)) {
                    cost -= dec;

                    if (is_approx_equal(cost, 0_vt, 0.001)) { cost = 0_vt; }

                    assert(cost >= 0_vt);
                }
            }

            /*
              compute_pattern generated a new pattern. Create/retrieve
              corresponding PDB, update collection size and reset
              time_point_of_last_new_pattern.
            */
            time_point_of_last_new_pattern = timer.get_elapsed_time();
            remaining_collection_size -= pdb.num_states();
            generated_pdbs.emplace_back(
                std::make_unique<ProbabilityAwarePatternDatabase>(
                    std::move(pdb)));
        }

        if (collection_size_limit_reached(remaining_collection_size) ||
            time_limit_reached(timer)) {
            break;
        }

        // Test if no new pattern was generated for longer than
        // stagnation_limit.
        if (timer.get_elapsed_time() - time_point_of_last_new_pattern >
            stagnation_limit_) {
            if (enable_blacklist_on_stagnation_) {
                if (blacklisting) {
                    if (log_.is_at_least_normal()) {
                        log_ << "stagnation limit reached "
                             << "despite blacklisting, terminating" << endl;
                    }
                    break;
                } else {
                    if (log_.is_at_least_normal()) {
                        log_ << "stagnation limit reached, "
                             << "enabling blacklisting" << endl;
                    }
                    blacklisting = true;
                    time_point_of_last_new_pattern = timer.get_elapsed_time();
                }
            } else {
                if (log_.is_at_least_normal()) {
                    log_ << "stagnation limit reached, terminating" << endl;
                }
                break;
            }
        }

        ++num_iterations;
        ++goal_index;
        goal_index = goal_index % goals.size();
        assert(utils::in_bounds(goal_index, goals));
    }

    PatternCollection patterns;
    patterns.reserve(generated_pdbs.size());
    for (const auto& gen_pdb : generated_pdbs) {
        patterns.push_back(gen_pdb->get_pattern());
    }

    std::shared_ptr<SubCollectionFinder> finder;

    if (use_saturated_costs_) {
        finder = std::make_shared<FullyAdditiveFinder>();
    } else {
        finder = std::make_shared<TrivialFinder>();
    }

    PatternCollectionInformation result(task, patterns, finder);
    result.set_pdbs(generated_pdbs);

    if (log_.is_at_least_normal()) {
        log_ << implementation_name_
             << " number of iterations: " << num_iterations << endl;
        log_ << implementation_name_ << " average time per generator: "
             << timer.get_elapsed_time() / num_iterations << endl;
    }

    return result;
}

} // namespace probfd::pdbs
