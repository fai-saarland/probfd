#include "probfd/pdbs/pattern_collection_generator_multiple.h"

#include "probfd/pdbs/fully_additive_finder.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/pdbs/saturation.h"
#include "probfd/pdbs/trivial_finder.h"

#include "probfd/cost_function.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/rng.h"

#include <cassert>
#include <ostream>
#include <set>
#include <span>
#include <vector>

using namespace std;

namespace probfd::pdbs {

namespace {

vector<FactPair> get_goals_in_random_order(
    const ProbabilisticTaskProxy& task_proxy,
    utils::RandomNumberGenerator& rng)
{
    vector<FactPair> goals =
        ::task_properties::get_fact_pairs(task_proxy.get_goals());
    rng.shuffle(goals);
    return goals;
}

vector<int> get_non_goal_variables(const ProbabilisticTaskProxy& task_proxy)
{
    size_t num_vars = task_proxy.get_variables().size();
    GoalsProxy goals = task_proxy.get_goals();
    vector<bool> is_goal(num_vars, false);
    for (FactProxy goal : goals) {
        is_goal[goal.get_variable().get_id()] = true;
    }

    vector<int> non_goal_variables;
    non_goal_variables.reserve(num_vars - goals.size());
    for (int var_id = 0; var_id < static_cast<int>(num_vars); ++var_id) {
        if (!is_goal[var_id]) {
            non_goal_variables.push_back(var_id);
        }
    }
    return non_goal_variables;
}

class ExplicitTaskCostFunction : public FDRSimpleCostFunction {
    const ProbabilisticTaskProxy& task_proxy;
    std::vector<value_t> costs;
    const value_t goal_termination;
    const value_t non_goal_termination;

public:
    ExplicitTaskCostFunction(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& cost_function)
        : task_proxy(task_proxy)
        , goal_termination(cost_function.get_goal_termination_cost())
        , non_goal_termination(cost_function.get_non_goal_termination_cost())
    {
        const auto operators = task_proxy.get_operators();
        costs.reserve(operators.size());

        for (const ProbabilisticOperatorProxy op : operators) {
            costs.push_back(
                cost_function.get_action_cost(OperatorID(op.get_id())));
        }
    }

    value_t get_action_cost(OperatorID op_id) override
    {
        return costs[op_id.get_index()];
    }

    [[nodiscard]]
    bool is_goal(const State& state) const override
    {
        return ::task_properties::is_goal_state(task_proxy, state);
    }

    [[nodiscard]]
    value_t get_goal_termination_cost() const override
    {
        return goal_termination;
    }

    [[nodiscard]]
    value_t get_non_goal_termination_cost() const override
    {
        return non_goal_termination;
    }

    void update_costs(const std::vector<value_t>& saturated_costs)
    {
        for (size_t i = 0; i != costs.size(); ++i) {
            costs[i] -= saturated_costs[i];
            assert(!is_approx_less(costs[i], 0.0_vt, 0.0001));

            // Avoid floating point imprecision. The PDB implementation is not
            // stable with respect to action costs very close to zero.
            if (is_approx_equal(costs[i], 0.0_vt, 0.0001)) {
                costs[i] = 0.0_vt;
            }
        }
    }
};

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
        if (log_.is_at_least_normal()) {
            log_ << "time limit reached" << endl;
        }
        return true;
    }
    return false;
}

PatternCollectionInformation PatternCollectionGeneratorMultiple::generate(
    const shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
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

    ProbabilisticTaskProxy task_proxy(*task);
    auto cost_function = std::make_shared<ExplicitTaskCostFunction>(
        task_proxy,
        *task_cost_function);

    utils::CountdownTimer timer(total_max_time_);

    // Store the set of goals in random order.
    vector<FactPair> goals = get_goals_in_random_order(task_proxy, *rng_);

    // Store the non-goal variables for potential blacklisting.
    vector<int> non_goal_variables = get_non_goal_variables(task_proxy);

    if (log_.is_at_least_debug()) {
        log_ << "goal variables: ";
        for (FactPair goal : goals) {
            log_ << goal.var << ", ";
        }
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
    std::vector<value_t> saturated_costs(task_proxy.get_operators().size());

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
                for (int var : blacklisted_variables) {
                    log_ << var << ", ";
                }
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
            task_proxy,
            cost_function,
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
                cost_function->update_costs(saturated_costs);
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

    PatternCollectionInformation result(
        task_proxy,
        task_cost_function,
        patterns,
        finder);

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
