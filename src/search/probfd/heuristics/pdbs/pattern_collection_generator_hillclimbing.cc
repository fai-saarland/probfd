#include "probfd/heuristics/pdbs/pattern_collection_generator_hillclimbing.h"

#include "probfd/heuristics/pdbs/incremental_ppdbs.h"
#include "probfd/heuristics/pdbs/probabilistic_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/cost_model.h"
#include "probfd/task_proxy.h"

#include "probfd/tasks/all_outcomes_determinization.h"
#include "probfd/tasks/root_task.h"

#include "probfd/task_utils/task_properties.h"

#include "algorithms/dynamic_bitset.h"

#include "pdbs/utils.h"

#include "utils/collections.h"
#include "utils/countdown_timer.h"
#include "utils/logging.h"
#include "utils/markup.h"
#include "utils/math.h"
#include "utils/memory.h"
#include "utils/rng.h"
#include "utils/rng_options.h"
#include "utils/timer.h"

#include "task_utils/causal_graph.h"
#include "task_utils/sampling.h"

#include "state_registry.h"

#include "option_parser.h"
#include "plugin.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

using namespace utils;

namespace probfd {

namespace heuristics {
namespace pdbs {

static std::vector<int> get_goal_variables(const TaskBaseProxy& task_proxy)
{
    std::vector<int> goal_vars;
    GoalsProxy goals = task_proxy.get_goals();
    goal_vars.reserve(goals.size());
    for (FactProxy goal : goals) {
        goal_vars.push_back(goal.get_variable().get_id());
    }
    assert(utils::is_sorted_unique(goal_vars));
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
static std::vector<std::vector<int>>
compute_relevant_neighbours(const ProbabilisticTask* task)
{
    const AbstractTask& determinization =
        task_properties::get_determinization(task);
    TaskProxy task_proxy(determinization);
    const causal_graph::CausalGraph& causal_graph =
        task_proxy.get_causal_graph();
    const std::vector<int> goal_vars = get_goal_variables(task_proxy);

    std::vector<std::vector<int>> connected_vars_by_variable;
    VariablesProxy variables = task_proxy.get_variables();
    connected_vars_by_variable.reserve(variables.size());
    for (VariableProxy var : variables) {
        int var_id = var.get_id();

        // Consider variables connected backwards via pre->eff arcs.
        const std::vector<int>& pre_to_eff_predecessors =
            causal_graph.get_eff_to_pre(var_id);

        // Consider goal variables connected (forwards) via eff--eff and
        // pre->eff arcs.
        const std::vector<int>& causal_graph_successors =
            causal_graph.get_successors(var_id);
        std::vector<int> goal_variable_successors;
        set_intersection(
            causal_graph_successors.begin(),
            causal_graph_successors.end(),
            goal_vars.begin(),
            goal_vars.end(),
            back_inserter(goal_variable_successors));

        // Combine relevant goal and non-goal variables.
        std::vector<int> relevant_neighbours;
        set_union(
            pre_to_eff_predecessors.begin(),
            pre_to_eff_predecessors.end(),
            goal_variable_successors.begin(),
            goal_variable_successors.end(),
            back_inserter(relevant_neighbours));

        connected_vars_by_variable.push_back(std::move(relevant_neighbours));
    }
    return connected_vars_by_variable;
}

struct PatternCollectionGeneratorHillclimbing::Sample {
    State state;
    EvaluationResult h_eval;
};

void PatternCollectionGeneratorHillclimbing::Statistics::print(
    std::ostream& out) const
{
    out << "\n"
        << "Hill Climbing Generator Statistics:"
        << "\n  Iterations: " << num_iterations
        << "\n  Generated patterns: " << generated_patterns
        << "\n  Rejected patterns: " << rejected_patterns
        << "\n  Maximum candidate PDB size: " << max_pdb_size
        << "\n  Time: " << hillclimbing_time << "s" << std::endl;
}

PatternCollectionGeneratorHillclimbing::PatternCollectionGeneratorHillclimbing(
    const Options& opts)
    : verbosity(opts.get<Verbosity>("verbosity"))
    , initial_generator(opts.get<std::shared_ptr<PatternCollectionGenerator>>(
          "initial_generator"))
    , subcollection_finder_factory(
          opts.get<std::shared_ptr<SubCollectionFinderFactory>>(
              "subcollection_finder_factory"))
    , pdb_max_size(opts.get<int>("pdb_max_size"))
    , collection_max_size(opts.get<int>("collection_max_size"))
    , num_samples(opts.get<int>("num_samples"))
    , min_improvement(opts.get<int>("min_improvement"))
    , max_time(opts.get<double>("max_time"))
    , rng(utils::parse_rng_from_options(opts))
    , remaining_states(opts.get<int>("search_space_max_size"))
    , num_rejected(0)
{
}

PatternCollectionGeneratorHillclimbing::
    ~PatternCollectionGeneratorHillclimbing() = default;

unsigned int PatternCollectionGeneratorHillclimbing::generate_candidate_pdbs(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    utils::CountdownTimer& hill_climbing_timer,
    const std::vector<std::vector<int>>& relevant_neighbours,
    const ProbabilisticPatternDatabase& pdb,
    std::set<DynamicBitset>& generated_patterns,
    PPDBCollection& candidate_pdbs)
{
    using namespace utils;

    if (verbosity >= Verbosity::VERBOSE) {
        std::cout << "Generating pattern neighborhood..." << std::endl;
    }

    const VariablesProxy variables = task_proxy.get_variables();
    const Pattern& pattern = pdb.get_pattern();
    unsigned int pdb_size = pdb.num_states();
    unsigned int max_pdb_size = 0;

    for (int pattern_var : pattern) {
        assert(utils::in_bounds(pattern_var, relevant_neighbours));
        const std::vector<int>& connected_vars =
            relevant_neighbours[pattern_var];

        // Only use variables which are not already in the pattern.
        std::vector<int> relevant_vars;
        std::ranges::set_difference(
            connected_vars,
            pattern,
            std::back_inserter(relevant_vars));

        for (int rel_var_id : relevant_vars) {
            hill_climbing_timer.throw_if_expired();

            const int rel_var_size = variables[rel_var_id].get_domain_size();
            const int max_size = std::min(pdb_max_size, remaining_states);

            if (!is_product_within_limit(pdb_size, rel_var_size, max_size)) {
                ++num_rejected;

                if (verbosity >= Verbosity::VERBOSE) {
                    std::cout << "Skipping neighboring pattern for variable "
                              << rel_var_id
                              << " because its PDB would exceed size limits."
                              << std::endl;
                }

                continue;
            }

            DynamicBitset bitset(variables.size());

            for (int var : pattern) {
                bitset.set(static_cast<size_t>(var));
            }

            bitset.set(static_cast<size_t>(rel_var_id));

            if (!generated_patterns.insert(bitset).second) {
                if (verbosity >= Verbosity::VERBOSE) {
                    std::cout << "Skipping neighboring pattern for variable "
                              << rel_var_id << " because it already exists."
                              << std::endl;
                }

                continue;
            }

            if (verbosity >= Verbosity::VERBOSE) {
                std::cout
                    << "Generating neighboring PDB for pattern with variable "
                    << rel_var_id << std::endl;
            }

            /*
                If we haven't seen this pattern before, generate a PDB
                for it and add it to candidate_pdbs if its size does not
                surpass the size limit.
            */
            auto& new_pdb =
                candidate_pdbs.emplace_back(new ProbabilisticPatternDatabase(
                    task_proxy,
                    pdb,
                    rel_var_id,
                    task_cost_function,
                    task_proxy.get_initial_state(),
                    true,
                    hill_climbing_timer.get_remaining_time()));
            const unsigned int num_states = new_pdb->num_states();
            max_pdb_size = std::max(max_pdb_size, num_states);
            remaining_states -= num_states;
        }
    }

    return max_pdb_size;
}

void PatternCollectionGeneratorHillclimbing::sample_states(
    utils::CountdownTimer& hill_climbing_timer,
    IncrementalPPDBs& current_pdbs,
    const sampling::RandomWalkSampler& sampler,
    value_t init_h,
    std::vector<Sample>& samples)
{
    assert(samples.empty());

    for (int i = 0; i < num_samples; ++i) {
        auto f = [&current_pdbs](const State& state) {
            return current_pdbs.is_dead_end(state);
        };

        // TODO How to choose the length of the random walk in MaxProb?
        State sample = sampler.sample_state(static_cast<int>(init_h), f);

        hill_climbing_timer.throw_if_expired();

        EvaluationResult eval = current_pdbs.evaluate(sample);
        samples.emplace_back(std::move(sample), eval);

        hill_climbing_timer.throw_if_expired();
    }
}

std::pair<int, int>
PatternCollectionGeneratorHillclimbing::find_best_improving_pdb(
    utils::CountdownTimer& hill_climbing_timer,
    IncrementalPPDBs& current_pdbs,
    const std::vector<Sample>& samples,
    PPDBCollection& candidate_pdbs)
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
        hill_climbing_timer.throw_if_expired();

        const auto& pdb = candidate_pdbs[i];
        if (!pdb) {
            /* candidate pattern is too large or has already been added to
               the canonical heuristic. */
            continue;
        }
        /*
          If a candidate's size added to the current collection's size exceeds
          the maximum collection size, then forget the pdb.
        */
        int combined_size = current_pdbs.get_size() + pdb->num_states();
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
        std::vector<PatternSubCollection> pattern_subcollections =
            current_pdbs.get_pattern_subcollections(pdb->get_pattern());
        for (const auto& sample : samples) {
            if (is_heuristic_improved(
                    *pdb,
                    sample,
                    *current_pdbs.get_pattern_databases(),
                    pattern_subcollections,
                    current_pdbs)) {
                ++count;
            }
        }

        if (count > improvement) {
            improvement = count;
            best_pdb_index = i;
        }

        if (verbosity >= Verbosity::VERBOSE && count > 0) {
            std::cout << "pattern: " << candidate_pdbs[i]->get_pattern()
                      << " - improvement: " << count << std::endl;
        }
    }

    return std::make_pair(improvement, best_pdb_index);
}

bool PatternCollectionGeneratorHillclimbing::is_heuristic_improved(
    const ProbabilisticPatternDatabase& pdb,
    const Sample& sample,
    const PPDBCollection& pdbs,
    const std::vector<PatternSubCollection>& pattern_subcollections,
    const IncrementalPPDBs& current_pdbs)
{
    const EvaluationResult h_pattern_eval = pdb.evaluate(sample.state);

    if (h_pattern_eval.is_unsolvable()) {
        return true;
    }

    const value_t h_pattern = h_pattern_eval.get_estimate();

    // h_collection: h-value of the current collection heuristic
    const EvaluationResult h_collection_eval = sample.h_eval;

    if (h_collection_eval.is_unsolvable()) return false;

    const value_t h_collection = h_collection_eval.get_estimate();

    std::vector<value_t> h_values;
    h_values.reserve(pdbs.size());

    for (const auto& p : pdbs) {
        const EvaluationResult eval = p->evaluate(sample.state);
        if (eval.is_unsolvable()) return false;
        h_values.push_back(eval.get_estimate());
    }

    for (const PatternSubCollection& subcollection : pattern_subcollections) {
        value_t h_subcollection =
            current_pdbs.evaluate_subcollection(h_values, subcollection);

        if (current_pdbs.combine(h_subcollection, h_pattern) > h_collection) {
            /*
              return true if a pattern clique is found for
              which the condition is met
            */
            return true;
        }
    }

    return false;
}

void PatternCollectionGeneratorHillclimbing::hill_climbing(
    const ProbabilisticTask* task,
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction& task_cost_function,
    IncrementalPPDBs& current_pdbs)
{
    int num_iterations = 0;
    utils::CountdownTimer hill_climbing_timer(max_time);

    const PatternCollection relevant_neighbours =
        compute_relevant_neighbours(task);

    // Candidate patterns generated so far (used to avoid duplicates).
    std::set<DynamicBitset> generated_patterns;
    // The PDBs for the patterns in generated_patterns that satisfy the size
    // limit to avoid recomputation.
    PPDBCollection candidate_pdbs;
    // The maximum size over all PDBs in candidate_pdbs.
    unsigned int max_pdb_size = 0;
    const int max_search_space_size = remaining_states;

    try {
        for (const auto& current_pdb : *current_pdbs.get_pattern_databases()) {
            unsigned int new_max_pdb_size = generate_candidate_pdbs(
                task_proxy,
                task_cost_function,
                hill_climbing_timer,
                relevant_neighbours,
                *current_pdb,
                generated_patterns,
                candidate_pdbs);
            max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);
        }
        /*
          NOTE: The initial set of candidate patterns (in generated_patterns) is
          guaranteed to be "normalized" in the sense that there are no
          duplicates and patterns are sorted.
        */
        if (verbosity >= Verbosity::NORMAL) {
            std::cout << "Done calculating initial candidate PDBs" << std::endl;
        }

        State initial_state = task_proxy.get_initial_state();

        const AbstractTask& aod = task_properties::get_determinization(task);
        TaskProxy aod_task_proxy(aod);

        sampling::RandomWalkSampler sampler(aod_task_proxy, *rng);
        std::vector<Sample> samples;
        samples.reserve(num_samples);

        while (true) {
            ++num_iterations;
            value_t init_h = current_pdbs.get_value(initial_state);
            const bool initial_dead = current_pdbs.is_dead_end(initial_state);

            if (verbosity >= Verbosity::VERBOSE) {
                std::cout << "current collection size is "
                          << current_pdbs.get_size() << "\n"
                          << "current search space size is "
                          << max_search_space_size - remaining_states
                          << "\ncurrent initial h value: ";

                if (initial_dead) {
                    std::cout << "infinite => stopping hill climbing"
                              << std::endl;
                } else {
                    std::cout << init_h << std::endl;
                }
            }

            if (initial_dead) {
                break;
            }

            sample_states(
                hill_climbing_timer,
                current_pdbs,
                sampler,
                init_h,
                samples);

            const auto [improvement, best_pdb_index] = find_best_improving_pdb(
                hill_climbing_timer,
                current_pdbs,
                samples,
                candidate_pdbs);

            samples.clear();

            if (improvement < min_improvement) {
                if (verbosity >= Verbosity::VERBOSE) {
                    std::cout << "Improvement below threshold."
                                 "Stop hill climbing."
                              << std::endl;
                }

                break;
            }

            // Add the best PDB to the CanonicalPDBsHeuristic.
            assert(best_pdb_index != -1);
            const auto best_pdb = candidate_pdbs[best_pdb_index];
            const Pattern& best_pattern = best_pdb->get_pattern();

            if (verbosity >= Verbosity::VERBOSE) {
                std::cout << "found a better pattern with improvement "
                          << improvement << std::endl;
                std::cout << "pattern: " << best_pattern << std::endl;
            }

            current_pdbs.add_pdb(best_pdb);

            // Generate candidate patterns and PDBs for next iteration.
            unsigned int new_max_pdb_size = generate_candidate_pdbs(
                task_proxy,
                task_cost_function,
                hill_climbing_timer,
                relevant_neighbours,
                *best_pdb,
                generated_patterns,
                candidate_pdbs);

            max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);

            // Remove the added PDB from candidate_pdbs.
            candidate_pdbs[best_pdb_index] = nullptr;

            if (verbosity >= Verbosity::VERBOSE) {
                std::cout << "Hill climbing time so far: "
                          << hill_climbing_timer.get_elapsed_time()
                          << std::endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (verbosity >= Verbosity::SILENT) {
            std::cout << "Time limit reached. Abort hill climbing."
                      << std::endl;
        }
    }

    statistics_.reset(new Statistics(
        num_iterations,
        generated_patterns.size(),
        num_rejected,
        max_pdb_size,
        hill_climbing_timer.get_elapsed_time()));

    if (verbosity >= Verbosity::NORMAL) {
        statistics_->print(std::cout);
    }
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::generate(
    const std::shared_ptr<ProbabilisticTask>& task)
{
    utils::Timer timer;

    if (verbosity >= Verbosity::NORMAL) {
        std::cout << "Generating patterns using the hill climbing generator..."
                  << std::endl;
    }

    // Generate initial collection
    assert(initial_generator);

    ProbabilisticTaskProxy task_proxy(*task);
    TaskCostFunction* task_cost_function = g_cost_model->get_cost_function();

    auto collection = initial_generator->generate(task);
    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory->create_subcollection_finder(task_proxy);

    IncrementalPPDBs current_pdbs(
        task_proxy,
        task_cost_function,
        collection,
        subcollection_finder);

    if (verbosity >= Verbosity::NORMAL) {
        std::cout << "Done calculating initial pattern collection: " << timer
                  << std::endl;
    }

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();
    if (!current_pdbs.is_dead_end(initial_state) && max_time > 0) {
        hill_climbing(
            task.get(),
            task_proxy,
            *task_cost_function,
            current_pdbs);
    }

    PatternCollectionInformation pci =
        current_pdbs.get_pattern_collection_information();

    return pci;
}

std::shared_ptr<utils::Printable>
PatternCollectionGeneratorHillclimbing::get_report() const
{
    return statistics_;
}

void add_hillclimbing_options(OptionParser& parser)
{
    utils::add_log_options_to_parser(parser);

    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "initial_generator",
        "generator for the initial pattern database ",
        "det_adapter(generator=systematic(pattern_max_size=1))");

    parser.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");

    parser.add_option<int>(
        "pdb_max_size",
        "maximal number of states per pattern database ",
        "2M",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "collection_max_size",
        "maximal number of states in the pattern collection",
        "10M",
        Bounds("1", "infinity"));
    parser.add_option<int>(
        "search_space_max_size",
        "maximal number of states in the pattern search space",
        "30M",
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

    utils::add_rng_options(parser);
}

void check_hillclimbing_options(OptionParser& parser, const Options& opts)
{
    if (opts.get<int>("min_improvement") > opts.get<int>("num_samples"))
        parser.error("minimum improvement must not be higher than number of "
                     "samples");
}

static std::shared_ptr<PatternCollectionGenerator> _parse(OptionParser& parser)
{
    add_hillclimbing_options(parser);

    Options opts = parser.parse();
    if (parser.help_mode()) return nullptr;

    check_hillclimbing_options(parser, opts);
    if (parser.dry_run()) return nullptr;

    return std::make_shared<PatternCollectionGeneratorHillclimbing>(opts);
}

static Plugin<PatternCollectionGenerator>
    _plugin("hillclimbing_probabilistic", _parse);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
