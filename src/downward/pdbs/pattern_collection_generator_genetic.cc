#include "downward/pdbs/pattern_collection_generator_genetic.h"

#include "downward/pdbs/zero_one_pdbs.h"

#include "downward/task_proxy.h"

#include "downward/task_utils/causal_graph.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

namespace downward::pdbs {
PatternCollectionGeneratorGenetic::PatternCollectionGeneratorGenetic(
    int pdb_max_size,
    int num_collections,
    int num_episodes,
    double mutation_probability,
    bool disjoint,
    int random_seed,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , pdb_max_size(pdb_max_size)
    , num_collections(num_collections)
    , num_episodes(num_episodes)
    , mutation_probability(mutation_probability)
    , disjoint_patterns(disjoint)
    , rng(utils::get_rng(random_seed))
{
}

void PatternCollectionGeneratorGenetic::select(
    const vector<double>& fitness_values)
{
    vector<double> cumulative_fitness;
    cumulative_fitness.reserve(fitness_values.size());
    double total_so_far = 0;
    for (double fitness_value : fitness_values) {
        total_so_far += fitness_value;
        cumulative_fitness.push_back(total_so_far);
    }
    // total_so_far is now sum over all fitness values.

    vector<vector<vector<bool>>> new_pattern_collections;
    new_pattern_collections.reserve(num_collections);
    for (int i = 0; i < num_collections; ++i) {
        int selected;
        if (total_so_far == 0) {
            // All fitness values are 0 => choose uniformly.
            selected = rng->random(fitness_values.size());
        } else {
            // [0..total_so_far)
            double random = rng->random() * total_so_far;
            // Find first entry which is strictly greater than random.
            selected = upper_bound(
                           cumulative_fitness.begin(),
                           cumulative_fitness.end(),
                           random) -
                       cumulative_fitness.begin();
        }
        new_pattern_collections.push_back(pattern_collections[selected]);
    }
    pattern_collections.swap(new_pattern_collections);
}

void PatternCollectionGeneratorGenetic::mutate()
{
    for (auto& collection : pattern_collections) {
        for (vector<bool>& pattern : collection) {
            for (size_t k = 0; k < pattern.size(); ++k) {
                double random = rng->random(); // [0..1)
                if (random < mutation_probability) {
                    pattern[k].flip();
                }
            }
        }
    }
}

Pattern PatternCollectionGeneratorGenetic::transform_to_pattern_normal_form(
    const vector<bool>& bitvector) const
{
    Pattern pattern;
    for (size_t i = 0; i < bitvector.size(); ++i) {
        if (bitvector[i]) pattern.push_back(i);
    }
    return pattern;
}

void PatternCollectionGeneratorGenetic::remove_irrelevant_variables(
    Pattern& pattern) const
{
    TaskProxy task_proxy(*task);

    unordered_set<int> in_original_pattern(pattern.begin(), pattern.end());
    unordered_set<int> in_pruned_pattern;

    vector<int> vars_to_check;
    for (FactProxy goal : task_proxy.get_goals()) {
        int var_id = goal.get_variable().get_id();
        if (in_original_pattern.count(var_id)) {
            // Goals are causally relevant.
            vars_to_check.push_back(var_id);
            in_pruned_pattern.insert(var_id);
        }
    }

    while (!vars_to_check.empty()) {
        int var = vars_to_check.back();
        vars_to_check.pop_back();
        /*
          A variable is relevant to the pattern if it is a goal variable or if
          there is a pre->eff arc from the variable to a relevant variable.
          Note that there is no point in considering eff->eff arcs here.
        */
        const causal_graph::CausalGraph& cg = task_proxy.get_causal_graph();

        const vector<int>& rel = cg.get_eff_to_pre(var);
        for (size_t i = 0; i < rel.size(); ++i) {
            int var_no = rel[i];
            if (in_original_pattern.count(var_no) &&
                !in_pruned_pattern.count(var_no)) {
                // Parents of relevant variables are causally relevant.
                vars_to_check.push_back(var_no);
                in_pruned_pattern.insert(var_no);
            }
        }
    }

    pattern.assign(in_pruned_pattern.begin(), in_pruned_pattern.end());
    sort(pattern.begin(), pattern.end());
}

bool PatternCollectionGeneratorGenetic::is_pattern_too_large(
    const Pattern& pattern) const
{
    // Test if the pattern respects the memory limit.
    TaskProxy task_proxy(*task);
    VariablesProxy variables = task_proxy.get_variables();
    int mem = 1;
    for (size_t i = 0; i < pattern.size(); ++i) {
        VariableProxy var = variables[pattern[i]];
        int domain_size = var.get_domain_size();
        if (!utils::is_product_within_limit(mem, domain_size, pdb_max_size))
            return true;
        mem *= domain_size;
    }
    return false;
}

bool PatternCollectionGeneratorGenetic::mark_used_variables(
    const Pattern& pattern,
    vector<bool>& variables_used) const
{
    for (size_t i = 0; i < pattern.size(); ++i) {
        int var_id = pattern[i];
        if (variables_used[var_id]) return true;
        variables_used[var_id] = true;
    }
    return false;
}

void PatternCollectionGeneratorGenetic::evaluate(vector<double>& fitness_values)
{
    TaskProxy task_proxy(*task);
    for (size_t i = 0; i < pattern_collections.size(); ++i) {
        const auto& collection = pattern_collections[i];
        if (log.is_at_least_debug()) {
            log << "evaluate pattern collection " << (i + 1) << " of "
                << pattern_collections.size() << endl;
        }
        double fitness = 0;
        bool pattern_valid = true;
        vector<bool> variables_used(task_proxy.get_variables().size(), false);
        shared_ptr<PatternCollection> pattern_collection =
            make_shared<PatternCollection>();
        pattern_collection->reserve(collection.size());
        for (const vector<bool>& bitvector : collection) {
            Pattern pattern = transform_to_pattern_normal_form(bitvector);

            if (is_pattern_too_large(pattern)) {
                if (log.is_at_least_verbose()) {
                    log << "pattern exceeds the memory limit!" << endl;
                }
                pattern_valid = false;
                break;
            }

            if (disjoint_patterns) {
                if (mark_used_variables(pattern, variables_used)) {
                    if (log.is_at_least_verbose()) {
                        log << "patterns are not disjoint anymore!" << endl;
                    }
                    pattern_valid = false;
                    break;
                }
            }

            remove_irrelevant_variables(pattern);
            pattern_collection->push_back(pattern);
        }
        if (!pattern_valid) {
            /* Set fitness to a very small value to cover cases in which all
               patterns are invalid. */
            fitness = 0.001;
        } else {
            /* Generate the pattern collection heuristic and get its fitness
               value. */
            ZeroOnePDBs zero_one_pdbs(task_proxy, *pattern_collection);
            fitness = zero_one_pdbs.compute_approx_mean_finite_h();
            // Update the best heuristic found so far.
            if (fitness > best_fitness) {
                best_fitness = fitness;
                if (log.is_at_least_normal()) {
                    log << "best_fitness = " << best_fitness << endl;
                }
                best_patterns = pattern_collection;
            }
        }
        fitness_values.push_back(fitness);
    }
}

void PatternCollectionGeneratorGenetic::bin_packing()
{
    TaskProxy task_proxy(*task);
    VariablesProxy variables = task_proxy.get_variables();

    vector<int> variable_ids;
    variable_ids.reserve(variables.size());
    for (size_t i = 0; i < variables.size(); ++i) {
        variable_ids.push_back(i);
    }

    for (int i = 0; i < num_collections; ++i) {
        // Use random variable ordering for all pattern collections.
        rng->shuffle(variable_ids);
        vector<vector<bool>> pattern_collection;
        vector<bool> pattern(variables.size(), false);
        int current_size = 1;
        for (size_t j = 0; j < variable_ids.size(); ++j) {
            int var_id = variable_ids[j];
            int next_var_size = variables[var_id].get_domain_size();
            if (next_var_size > pdb_max_size)
                // var never fits into a bin.
                continue;
            if (!utils::is_product_within_limit(
                    current_size,
                    next_var_size,
                    pdb_max_size)) {
                // Open a new bin for var.
                pattern_collection.push_back(pattern);
                pattern.clear();
                pattern.resize(variables.size(), false);
                current_size = 1;
            }
            current_size *= next_var_size;
            pattern[var_id] = true;
        }
        /*
          The last bin has not been inserted into pattern_collection, do
          so now. We test current_size against 1 because this is cheaper
          than testing if pattern is an all-zero bitvector. current_size
          can only be 1 if *all* variables have a domain larger than
          pdb_max_size.
        */
        if (current_size > 1) {
            pattern_collection.push_back(pattern);
        }
        pattern_collections.push_back(pattern_collection);
    }
}

void PatternCollectionGeneratorGenetic::genetic_algorithm()
{
    best_fitness = -1;
    best_patterns = nullptr;
    bin_packing();
    vector<double> initial_fitness_values;
    evaluate(initial_fitness_values);
    for (int i = 0; i < num_episodes; ++i) {
        if (log.is_at_least_verbose()) {
            log << "--------- episode no " << (i + 1) << " ---------" << endl;
        }
        mutate();
        vector<double> fitness_values;
        evaluate(fitness_values);
        // We allow to select invalid pattern collections.
        select(fitness_values);
    }
}

string PatternCollectionGeneratorGenetic::name() const
{
    return "genetic pattern collection generator";
}

PatternCollectionInformation
PatternCollectionGeneratorGenetic::compute_patterns(
    const shared_ptr<AbstractTask>& task_)
{
    task = task_;
    genetic_algorithm();

    TaskProxy task_proxy(*task);
    assert(best_patterns);
    return PatternCollectionInformation(task_proxy, best_patterns, log);
}

} // namespace pdbs
