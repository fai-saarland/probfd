#include "pattern_collection_generator_hillclimbing.h"

#include "../expcost_projection.h"
#include "incremental_canonical_pdbs.h"

#include "../../../../../option_parser.h"
#include "../../../../../plugin.h"

#include "../../../../../globals.h"
#include "../../../../../global_state.h"
#include "../../../../../state_registry.h"
#include "../../../../../causal_graph.h"
#include "../../../../../sampling.h"
#include "../../../../../utils/collections.h"
#include "../../../../../utils/countdown_timer.h"
#include "../../../../../utils/logging.h"
#include "../../../../../utils/markup.h"
#include "../../../../../utils/math.h"
#include "../../../../../utils/memory.h"
#include "../../../../../utils/rng.h"
#include "../../../../../utils/rng_options.h"
#include "../../../../../utils/timer.h"
#include "../../../../../pdbs/utils.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

#define ECHC_VERBOSE

#if defined DEBUG || defined ECHC_VERBOSE
static constexpr bool VERBOSE = true;
#else
static constexpr bool VERBOSE = false;
#endif

namespace probabilistic {

using namespace value_type;

namespace pdbs {
namespace pattern_selection {

/* Since this exception class is only used for control flow and thus has no need
   for an error message, we use a standalone class instead of inheriting from
   utils::Exception. */
class HillClimbingTimeout {
};

static std::vector<int> get_goal_variables() {
    std::vector<int> goal_vars;
    for (const auto& g : g_goal) { 
        goal_vars.push_back(g.first);
    }
    std::sort(goal_vars.begin(), goal_vars.end());
    return goal_vars;
}

/*
  When growing a pattern, we only want to consider successor patterns
  that are *interesting*. A pattern is interesting if the subgraph of
  the causal graph induced by the pattern satisfies the following two
  properties:
  A. it is weakly connected (considering all kinds of arcs)
  B. from every variable in the pattern, a goal variable is reachable by a
     path that only uses pre->eff arcs

  We can use the assumption that the pattern we want to extend is
  already interesting, so the question is how an interesting pattern
  can be obtained from an interesting pattern by adding one variable.

  There are two ways to do this:
  1. Add a *predecessor* of an existing variable along a pre->eff arc.
  2. Add any *goal variable* that is a weakly connected neighbour of an
     existing variable (using any kind of arc).

  Note that in the iPDB paper, the second case was missed. Adding it
  significantly helps with performance in our experiments (see
  issue743, msg6595).

  In our implementation, for efficiency we replace condition 2. by
  only considering causal graph *successors* (along either pre->eff or
  eff--eff arcs), because these can be obtained directly, and the
  missing case (predecessors along pre->eff arcs) is already covered
  by the first condition anyway.

  This method precomputes all variables which satisfy conditions 1. or
  2. for a given neighbour variable already in the pattern.
*/
static std::vector<std::vector<int>> compute_relevant_neighbours() {
    const CausalGraph &causal_graph = *g_causal_graph;
    const std::vector<int> goal_vars = get_goal_variables();

    std::vector<std::vector<int>> connected_vars_by_variable;
    connected_vars_by_variable.reserve(g_variable_domain.size());

    const int num_variables = static_cast<int>(g_variable_domain.size());
    for (int var_id = 0; var_id < num_variables; ++var_id) {
        // Consider variables connected backwards via pre->eff arcs.
        const std::vector<int> &pre_to_eff_predecessors =
            causal_graph.get_eff_to_pre(var_id);

        // Consider goal variables connected (forwards) via eff--eff and 
        // pre->eff arcs.
        const std::vector<int> &causal_graph_successors =
            causal_graph.get_successors(var_id);

        std::vector<int> relevant_neighbours =
            utils::set_intersection(causal_graph_successors, goal_vars);

        // Combine relevant goal and non-goal variables.
        utils::insert_set(relevant_neighbours, pre_to_eff_predecessors);

        connected_vars_by_variable.push_back(move(relevant_neighbours));
    }

    return connected_vars_by_variable;
}


PatternCollectionGeneratorHillclimbing::
PatternCollectionGeneratorHillclimbing(const Options &opts)
    : pdb_max_size(opts.get<int>("pdb_max_size")),
      collection_max_size(opts.get<int>("collection_max_size")),
      num_samples(opts.get<int>("num_samples")),
      min_improvement(opts.get<int>("min_improvement")),
      max_time(opts.get<double>("max_time")),
      rng(utils::parse_rng_from_options(opts)),
      num_rejected(0),
      hill_climbing_timer(0),
      cost_type(OperatorCost(opts.get_enum("cost_type")))
{
}

int PatternCollectionGeneratorHillclimbing::generate_candidate_pdbs(
    const std::vector<std::vector<int>> &relevant_neighbours,
    const ExpCostProjection &pdb,
    std::set<Pattern> &generated_patterns,
    ExpCostPDBCollection &candidate_pdbs)
{
    const Pattern &pattern = pdb.get_pattern();
    int pdb_size = pdb.num_states();
    int max_pdb_size = 0;

    for (int pattern_var : pattern) {
        assert(utils::in_bounds(pattern_var, relevant_neighbours));
        const std::vector<int>& connected_vars =
            relevant_neighbours[pattern_var];

        // Only use variables which are not already in the pattern.
        std::vector relevant_vars =
            utils::set_difference(connected_vars, pattern);

        for (int rel_var_id : relevant_vars) {
            int rel_var_size = g_variable_domain[rel_var_id];
            if (utils::is_product_within_limit(
                pdb_size, rel_var_size, pdb_max_size))
            {
                Pattern new_pattern(pattern);
                utils::insert_set(new_pattern, rel_var_id);

                if (!utils::contains(generated_patterns, new_pattern)) {
                    /*
                      If we haven't seen this pattern before, generate a PDB
                      for it and add it to candidate_pdbs if its size does not
                      surpass the size limit.
                    */
                    auto& new_pdb = candidate_pdbs.emplace_back(
                        new ExpCostProjection(pdb, rel_var_id));
                    generated_patterns.insert(new_pattern);
                    max_pdb_size = std::max(
                        max_pdb_size, (int)new_pdb->num_states());
                }
            } else {
                ++num_rejected;
            }
        }
    }
    
    return max_pdb_size;
}

void PatternCollectionGeneratorHillclimbing::sample_states(
    const sampling::RandomWalkSampler &sampler,
    value_t init_h,
    std::vector<GlobalState> &samples)
{
    assert(samples.empty());

    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        auto f = [this](const GlobalState &state) {
            return current_pdbs->is_dead_end(state);
        };
        samples.push_back(sampler.sample_state(init_h, f));
        if (hill_climbing_timer->is_expired()) {
            throw HillClimbingTimeout();
        }
    }
}

std::pair<int, int>
PatternCollectionGeneratorHillclimbing::find_best_improving_pdb(
    const std::vector<GlobalState> &samples,
    const std::vector<value_t> &samples_h_values,
    ExpCostPDBCollection &candidate_pdbs)
{
    /*
      TODO: The original implementation by Haslum et al. uses A* to compute
      h values for the sample states only instead of generating all PDBs.
      improvement: best improvement (= highest count) for a pattern so far.
      We require that a pattern must have an improvement of at least one in
      order to be taken into account.
    */
    int improvement = 0;
    int best_pdb_index = -1;

    // Iterate over all candidates and search for the best improving pattern/pdb
    for (size_t i = 0; i < candidate_pdbs.size(); ++i) {
        if (hill_climbing_timer->is_expired())
            throw HillClimbingTimeout();

        const std::shared_ptr<ExpCostProjection> &pdb = candidate_pdbs[i];
        if (!pdb) {
            /* candidate pattern is too large or has already been added to
               the canonical heuristic. */
            continue;
        }
        /*
          If a candidate's size added to the current collection's size exceeds
          the maximum collection size, then forget the pdb.
        */
        int combined_size = current_pdbs->get_size() + pdb->num_states();
        if (combined_size > collection_max_size) {
            candidate_pdbs[i] = nullptr;
            continue;
        }

        /*
          Calculate the "counting approximation" for all sample states: count
          the number of samples for which the current pattern collection
          heuristic would be improved if the new pattern was included into it.
        */
        /*
          TODO: The original implementation by Haslum et al. uses m/t as a
          statistical confidence interval to stop the A*-search (which they use,
          see above) earlier.
        */
        int count = 0;
        std::vector<PatternClique> pattern_cliques =
            current_pdbs->get_pattern_cliques(pdb->get_pattern());
        for (int sample_id = 0; sample_id < num_samples; ++sample_id) {
            const GlobalState &sample = samples[sample_id];
            assert(utils::in_bounds(sample_id, samples_h_values));
            value_t h_collection = samples_h_values[sample_id];
            if (is_heuristic_improved(
                    *pdb, sample, h_collection,
                    *current_pdbs->get_pattern_databases(), pattern_cliques)) {
                ++count;
            }
        }
        
        if (count > improvement) {
            improvement = count;
            best_pdb_index = i;
        }

        if constexpr (VERBOSE) {
            if (count > 0) {
                std::cout
                    << "pattern: " << candidate_pdbs[i]->get_pattern()
                    << " - improvement: " << count << std::endl;
            }
        }
    }

    return std::make_pair(improvement, best_pdb_index);
}

bool PatternCollectionGeneratorHillclimbing::is_heuristic_improved(
    const ExpCostProjection &pdb,
    const GlobalState &sample,
    value_t h_collection,
    const ExpCostPDBCollection &pdbs,
    const std::vector<PatternClique> &pattern_cliques)
{
    // h_pattern: h-value of the new pattern
    value_t h_pattern = pdb.get_value(sample);

    if (h_pattern == -value_type::inf) {
        return true;
    }

    // h_collection: h-value of the current collection heuristic
    if (h_collection == -value_type::inf)
        return false;

    std::vector<value_t> h_values;
    h_values.reserve(pdbs.size());

    for (const std::shared_ptr<ExpCostProjection> &p : pdbs) {
        value_t h = p->get_value(sample);
        if (h == -value_type::inf)
            return false;
        h_values.push_back(h);
    }

    for (const PatternClique &clique : pattern_cliques) {
        value_t h_clique = value_type::zero;
        for (PatternID pattern_id : clique) {
            h_clique += h_values[pattern_id];
        }

        if (h_pattern + h_clique < h_collection) {
            /*
              return true if a pattern clique is found for
              which the condition is met
            */
            return true;
        }
    }

    return false;
}

void PatternCollectionGeneratorHillclimbing::hill_climbing() {
    utils::CountdownTimer timer(max_time);
    hill_climbing_timer = &timer;

    const PatternCollection relevant_neighbours =
        compute_relevant_neighbours();

    // Candidate patterns generated so far (used to avoid duplicates).
    std::set<Pattern> generated_patterns;
    // The PDBs for the patterns in generated_patterns that satisfy the size
    // limit to avoid recomputation.
    ExpCostPDBCollection candidate_pdbs;
    // The maximum size over all PDBs in candidate_pdbs.
    int max_pdb_size = 0;
    for (const auto &current_pdb : *current_pdbs->get_pattern_databases()) {
        int new_max_pdb_size = generate_candidate_pdbs(
            relevant_neighbours, *current_pdb, generated_patterns,
            candidate_pdbs);
        max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);
    }
    /*
      NOTE: The initial set of candidate patterns (in generated_patterns) is
      guaranteed to be "normalized" in the sense that there are no duplicates
      and patterns are sorted.
    */
    std::cout << "Done calculating initial candidate PDBs" << std::endl;

    int num_iterations = 0;
    StateRegistry state_registry;
    GlobalState initial_state = state_registry.get_initial_state();

    sampling::RandomWalkSampler sampler(state_registry, *rng);
    std::vector<GlobalState> samples;
    std::vector<value_t> samples_h_values;

    try {
        while (true) {
            ++num_iterations;
            value_t init_h = current_pdbs->get_value(initial_state);

            if constexpr (VERBOSE) {
                std::cout
                    << "current collection size is "
                    << current_pdbs->get_size()
                    << "\ncurrent initial h value: ";
            }

            if (current_pdbs->is_dead_end(initial_state)) {
                if constexpr (VERBOSE) {
                    std::cout
                        << "infinite => stopping hill climbing"
                        << std::endl;
                }
                break;
            } else if constexpr (VERBOSE) {
                std::cout << init_h << std::endl;
            }

            samples.clear();
            samples_h_values.clear();
            sample_states(sampler, init_h, samples);
            for (const GlobalState &sample : samples) {
                samples_h_values.push_back(current_pdbs->get_value(sample));
            }

            const auto [improvement, best_pdb_index] =
                find_best_improving_pdb(samples, samples_h_values, candidate_pdbs);

            if (improvement < min_improvement) {
                if constexpr (VERBOSE) {
                    std::cout <<
                        "Improvement below threshold."
                        "Stop hill climbing."
                        << std::endl;
                }

                break;
            }

            // Add the best PDB to the CanonicalPDBsHeuristic.
            assert(best_pdb_index != -1);
            const auto best_pdb = candidate_pdbs[best_pdb_index];
            const Pattern &best_pattern = best_pdb->get_pattern();

            if constexpr (VERBOSE) {
                std::cout << "found a better pattern with improvement "
                     << improvement << std::endl;
                std::cout << "pattern: " << best_pattern << std::endl;
            }

            current_pdbs->add_pdb(best_pdb);

            // Generate candidate patterns and PDBs for next iteration.
            int new_max_pdb_size = generate_candidate_pdbs(
                relevant_neighbours, *best_pdb, generated_patterns,
                candidate_pdbs);

            max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);

            // Remove the added PDB from candidate_pdbs.
            candidate_pdbs[best_pdb_index] = nullptr;

            std::cout
                << "Hill climbing time so far: "
                << hill_climbing_timer->get_elapsed_time()
                << std::endl;
        }
    } catch (HillClimbingTimeout &) {
        std::cout << "Time limit reached. Abort hill climbing." << std::endl;
    }

    std::cout
        << "Hill climbing iterations: " << num_iterations
        << "\nHill climbing generated patterns: " << generated_patterns.size()
        << "\nHill climbing rejected patterns: " << num_rejected
        << "\nHill climbing maximum PDB size: " << max_pdb_size
        << "\nHill climbing time: " << hill_climbing_timer->get_elapsed_time()
        << std::endl;

    hill_climbing_timer = nullptr;
}

static PatternCollection get_initial_pattern_collection() {
    // A pattern for each goal variable.
    PatternCollection initial_pattern_collection;
    for (const auto& goal : g_goal) {
        int goal_var_id = goal.first;
        initial_pattern_collection.emplace_back(1, goal_var_id);
    }
    return initial_pattern_collection;
}

PatternCollectionInformation
PatternCollectionGeneratorHillclimbing::generate(OperatorCost cost_type)
{
    this->cost_type = cost_type;

    utils::Timer timer;
    std::cout
        << "Generating patterns using the hill climbing generator..."
        << std::endl;

    // Generate initial collection
    current_pdbs = std::make_unique<IncrementalCanonicalPDBs>(
        get_initial_pattern_collection());

    if constexpr (VERBOSE) {
        std::cout
            << "Done calculating initial pattern collection: " << timer
            << std::endl;
    }

    GlobalState initial_state = g_initial_state();
    if (!current_pdbs->is_dead_end(initial_state) && max_time > 0) {
        hill_climbing();
    }

    PatternCollectionInformation pci =
        current_pdbs->get_pattern_collection_information();
    
    return pci;
}

void add_hillclimbing_options(OptionParser &parser) {
    parser.add_option<int>(
        "pdb_max_size",
        "maximal number of states per pattern database ",
        "2000000",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "collection_max_size",
        "maximal number of states in the pattern collection",
        "20000000",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "num_samples",
        "number of samples (random states) on which to evaluate each "
        "candidate pattern collection",
        "1000",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "min_improvement",
        "minimum number of samples on which a candidate pattern "
        "collection must improve on the current one to be considered "
        "as the next pattern collection ",
        "10",
        Bounds("1", "infinity"));
    parser.add_option<double>(
        "max_time",
        "maximum time in seconds for improving the initial pattern "
        "collection via hill climbing. If set to 0, no hill climbing "
        "is performed at all. Note that this limit only affects hill "
        "climbing. Use max_time_dominance_pruning to limit the time "
        "spent for pruning dominated patterns.",
        "infinity",
        Bounds("0.0", "infinity"));
    add_cost_type_option_to_parser(parser);
    utils::add_rng_options(parser);
}

void check_hillclimbing_options(
    OptionParser &parser, const Options &opts) {
    if (opts.get<int>("min_improvement") > opts.get<int>("num_samples"))
        parser.error("minimum improvement must not be higher than number of "
                     "samples");
}

static std::shared_ptr<PatternCollectionGenerator> _parse(OptionParser &parser) 
{
    add_hillclimbing_options(parser);

    Options opts = parser.parse();
    if (parser.help_mode())
        return nullptr;

    check_hillclimbing_options(parser, opts);
    if (parser.dry_run())
        return nullptr;

    return std::make_shared<PatternCollectionGeneratorHillclimbing>(opts);
}

static Plugin<PatternCollectionGenerator> _plugin("hillclimbing_ec", _parse);

}
}
}
