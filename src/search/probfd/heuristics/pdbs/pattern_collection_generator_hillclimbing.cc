#include "probfd/heuristics/pdbs/pattern_collection_generator_hillclimbing.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"

#include "probfd/cost_function.h"
#include "probfd/task_proxy.h"

#include "probfd/tasks/all_outcomes_determinization.h"
#include "probfd/tasks/root_task.h"

#include "probfd/task_utils/task_properties.h"

#include "downward/algorithms/dynamic_bitset.h"

#include "downward/pdbs/utils.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/markup.h"
#include "downward/utils/math.h"
#include "downward/utils/memory.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/timer.h"

#include "downward/task_utils/causal_graph.h"
#include "downward/task_utils/sampling.h"

#include "downward/state_registry.h"

#include "downward/plugins/plugin.h"

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

unsigned long long compute_total_pdb_size(const PPDBCollection& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) {
        size += pdb->num_states();
    }

    return size;
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
    value_t h;
};

class PatternCollectionGeneratorHillclimbing::IncrementalPPDBs {
    ProbabilisticTaskProxy task_proxy;
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function;

    std::shared_ptr<PatternCollection> patterns;
    std::shared_ptr<PPDBCollection> pattern_databases;
    std::shared_ptr<std::vector<PatternSubCollection>> pattern_subcollections;

    std::shared_ptr<SubCollectionFinder> subcollection_finder;

    // The sum of all abstract state sizes of all pdbs in the collection.
    long long size;

    // Adds a PDB for pattern but does not recompute pattern_subcollections.
    void add_pdb_for_pattern(const Pattern& pattern, const State& intial_state);

    void recompute_pattern_subcollections();

public:
    IncrementalPPDBs(
        const ProbabilisticTaskProxy& task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        const PatternCollection& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    IncrementalPPDBs(
        const ProbabilisticTaskProxy& task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        PatternCollectionInformation& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    // Adds a new PDB to the collection and recomputes pattern_subcollections.
    void add_pdb(const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb);

    int count_improvements(
        const ProbabilityAwarePatternDatabase& pdb,
        const std::vector<PatternCollectionGeneratorHillclimbing::Sample>&
            samples,
        value_t termination_cost) const;

    value_t evaluate(const State& state, value_t termination_cost) const;

    /*
      The following method offers a quick dead-end check for the sampling
      procedure of iPDB-hillclimbing. This exists because we can much more
      efficiently test if the canonical heuristic is infinite than
      computing the exact heuristic value.
    */
    bool is_dead_end(const State& state, value_t termination_cost) const;

    PatternCollectionInformation get_pattern_collection_information() const;

    std::shared_ptr<PPDBCollection> get_pattern_databases() const;

    long long get_size() const;

private:
    bool is_heuristic_improved(
        const ProbabilityAwarePatternDatabase& pdb,
        const PatternCollectionGeneratorHillclimbing::Sample& sample,
        const std::vector<PatternSubCollection>& pattern_subcollections,
        value_t termination_cost) const;
};

PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    const PatternCollection& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(std::move(task_cost_function))
    , patterns(new PatternCollection(initial_patterns))
    , pattern_databases(new PPDBCollection())
    , pattern_subcollections(nullptr)
    , subcollection_finder(subcollection_finder)
    , size(0)
{
    pattern_databases->reserve(patterns->size());
    for (const Pattern& pattern : *patterns)
        add_pdb_for_pattern(pattern, task_proxy.get_initial_state());
    recompute_pattern_subcollections();
}

PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    const ProbabilisticTaskProxy& task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    PatternCollectionInformation& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(std::move(task_cost_function))
    , patterns(initial_patterns.get_patterns())
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_subcollections(initial_patterns.get_subcollections())
    , subcollection_finder(subcollection_finder)
    , size(compute_total_pdb_size(*pattern_databases))
{
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    add_pdb_for_pattern(const Pattern& pattern, const State& initial_state)
{
    auto& pdb =
        pattern_databases->emplace_back(new ProbabilityAwarePatternDatabase(
            task_proxy,
            pattern,
            *task_cost_function,
            initial_state));
    size += pdb->num_states();
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::add_pdb(
    const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb)
{
    patterns->push_back(pdb->get_pattern());
    auto& new_pdb = pattern_databases->emplace_back(pdb);
    size += new_pdb->num_states();
    recompute_pattern_subcollections();
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    recompute_pattern_subcollections()
{
    pattern_subcollections =
        subcollection_finder->compute_subcollections(*patterns);
}

int PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    count_improvements(
        const ProbabilityAwarePatternDatabase& pdb,
        const std::vector<Sample>& samples,
        value_t termination_cost) const
{
    int count = 0;
    std::vector<PatternSubCollection> subcollections =
        subcollection_finder->compute_subcollections_with_pattern(
            *patterns,
            *pattern_subcollections,
            pdb.get_pattern());
    for (const auto& sample : samples) {
        if (is_heuristic_improved(
                pdb,
                sample,
                subcollections,
                termination_cost)) {
            ++count;
        }
    }

    return count;
}

value_t PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::evaluate(
    const State& state,
    value_t termination_cost) const
{
    return subcollection_finder->evaluate(
        *pattern_databases,
        *pattern_subcollections,
        state,
        termination_cost);
}

bool PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::is_dead_end(
    const State& state,
    value_t termination_cost) const
{
    for (const auto& pdb : *pattern_databases) {
        if (pdb->lookup_estimate(state) == termination_cost) {
            return true;
        }
    }

    return false;
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::
    IncrementalPPDBs::get_pattern_collection_information() const
{
    PatternCollectionInformation result(
        task_proxy,
        task_cost_function,
        patterns,
        subcollection_finder);
    result.set_pdbs(pattern_databases);
    result.set_subcollections(pattern_subcollections);
    return result;
}

std::shared_ptr<PPDBCollection> PatternCollectionGeneratorHillclimbing::
    IncrementalPPDBs::get_pattern_databases() const
{
    return pattern_databases;
}

long long
PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::get_size() const
{
    return size;
}

bool PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    is_heuristic_improved(
        const ProbabilityAwarePatternDatabase& pdb,
        const Sample& sample,
        const std::vector<PatternSubCollection>& pattern_subcollections,
        value_t termination_cost) const
{
    const value_t h_pattern = pdb.lookup_estimate(sample.state);

    if (h_pattern == termination_cost) {
        return true;
    }

    // h_collection: h-value of the current collection heuristic
    const value_t h_collection = sample.h;

    if (h_collection == termination_cost) return false;

    std::vector<value_t> h_values;
    h_values.reserve(pattern_databases->size());

    for (const auto& p : *pattern_databases) {
        const value_t h = p->lookup_estimate(sample.state);
        if (h == termination_cost) return false;
        h_values.push_back(h);
    }

    for (const PatternSubCollection& subcollection : pattern_subcollections) {
        const value_t h_subcollection =
            subcollection_finder->evaluate_subcollection(
                h_values,
                subcollection);

        const value_t combined =
            subcollection_finder->combine(h_subcollection, h_pattern);

        if (combined > h_collection) {
            /*
              return true if a pattern clique is found for
              which the condition is met
            */
            return true;
        }
    }

    return false;
}

PatternCollectionGeneratorHillclimbing::PatternCollectionGeneratorHillclimbing(
    const plugins::Options& opts)
    : PatternCollectionGenerator(opts)
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
    FDRSimpleCostFunction& task_cost_function,
    utils::CountdownTimer& hill_climbing_timer,
    const std::vector<std::vector<int>>& relevant_neighbours,
    const ProbabilityAwarePatternDatabase& pdb,
    std::set<DynamicBitset>& generated_patterns,
    PPDBCollection& candidate_pdbs)
{
    using namespace utils;

    if (log.is_at_least_verbose()) {
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

                if (log.is_at_least_verbose()) {
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
                if (log.is_at_least_verbose()) {
                    std::cout << "Skipping neighboring pattern for variable "
                              << rel_var_id << " because it already exists."
                              << std::endl;
                }

                continue;
            }

            if (log.is_at_least_verbose()) {
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
                candidate_pdbs.emplace_back(new ProbabilityAwarePatternDatabase(
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
    value_t termination_cost,
    std::vector<Sample>& samples)
{
    assert(samples.empty());

    for (int i = 0; i < num_samples; ++i) {
        auto f = [=, &current_pdbs](const State& state) {
            return current_pdbs.is_dead_end(state, termination_cost);
        };

        // TODO How to choose the length of the random walk in MaxProb?
        State sample = sampler.sample_state(static_cast<int>(init_h), f);

        hill_climbing_timer.throw_if_expired();

        value_t h = current_pdbs.evaluate(sample, termination_cost);
        samples.emplace_back(std::move(sample), h);

        hill_climbing_timer.throw_if_expired();
    }
}

std::pair<int, int>
PatternCollectionGeneratorHillclimbing::find_best_improving_pdb(
    utils::CountdownTimer& hill_climbing_timer,
    IncrementalPPDBs& current_pdbs,
    const std::vector<Sample>& samples,
    PPDBCollection& candidate_pdbs,
    value_t termination_cost)
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
        const int count =
            current_pdbs.count_improvements(*pdb, samples, termination_cost);

        if (count > improvement) {
            improvement = count;
            best_pdb_index = i;
        }

        if (log.is_at_least_verbose() && count > 0) {
            std::cout << "pattern: " << candidate_pdbs[i]->get_pattern()
                      << " - improvement: " << count << std::endl;
        }
    }

    return std::make_pair(improvement, best_pdb_index);
}

void PatternCollectionGeneratorHillclimbing::hill_climbing(
    const ProbabilisticTask* task,
    const ProbabilisticTaskProxy& task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    IncrementalPPDBs& current_pdbs)
{
    const value_t termination_cost =
        task_cost_function.get_non_goal_termination_cost();

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
        if (log.is_at_least_normal()) {
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
            value_t init_h =
                current_pdbs.evaluate(initial_state, termination_cost);
            const bool initial_dead = init_h == termination_cost;

            if (log.is_at_least_verbose()) {
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
                termination_cost,
                samples);

            const auto [improvement, best_pdb_index] = find_best_improving_pdb(
                hill_climbing_timer,
                current_pdbs,
                samples,
                candidate_pdbs,
                termination_cost);

            samples.clear();

            if (improvement < min_improvement) {
                if (log.is_at_least_verbose()) {
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

            if (log.is_at_least_verbose()) {
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

            if (log.is_at_least_verbose()) {
                std::cout << "Hill climbing time so far: "
                          << hill_climbing_timer.get_elapsed_time()
                          << std::endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (log.is_at_least_normal()) {
            std::cout << "Time limit reached. Abort hill climbing."
                      << std::endl;
        }
    }

    if (log.is_at_least_normal()) {
        log << "\n"
            << "Hill Climbing Generator Statistics:"
            << "\n  Iterations: " << num_iterations
            << "\n  Generated patterns: " << generated_patterns.size()
            << "\n  Rejected patterns: " << num_rejected
            << "\n  Maximum candidate PDB size: " << max_pdb_size
            << "\n  Time: " << hill_climbing_timer.get_elapsed_time() << "s"
            << std::endl;
    }
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::generate(
    const std::shared_ptr<ProbabilisticTask>& task,
    const std::shared_ptr<FDRCostFunction>& task_cost_function)
{
    utils::Timer timer;

    if (log.is_at_least_normal()) {
        std::cout << "Generating patterns using the hill climbing generator..."
                  << std::endl;
    }

    // Generate initial collection
    assert(initial_generator);

    ProbabilisticTaskProxy task_proxy(*task);

    auto collection = initial_generator->generate(task, task_cost_function);
    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory->create_subcollection_finder(task_proxy);

    IncrementalPPDBs current_pdbs(
        task_proxy,
        task_cost_function,
        collection,
        subcollection_finder);

    if (log.is_at_least_normal()) {
        std::cout << "Done calculating initial pattern collection: " << timer
                  << std::endl;
    }

    const State initial_state = task_proxy.get_initial_state();
    initial_state.unpack();

    const value_t termination_cost =
        task_cost_function->get_non_goal_termination_cost();

    value_t init_h = current_pdbs.evaluate(initial_state, termination_cost);

    if (init_h != termination_cost && max_time > 0) {
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

void add_hillclimbing_options(plugins::Feature& feature)
{
    feature.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "initial_generator",
        "generator for the initial pattern database ",
        "det_adapter(generator=systematic(pattern_max_size=1))");

    feature.add_option<std::shared_ptr<SubCollectionFinderFactory>>(
        "subcollection_finder_factory",
        "The subcollection finder factory.",
        "finder_trivial_factory()");

    feature.add_option<int>(
        "pdb_max_size",
        "maximal number of states per pattern database ",
        "2M",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "collection_max_size",
        "maximal number of states in the pattern collection",
        "10M",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "search_space_max_size",
        "maximal number of states in the pattern search space",
        "30M",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "num_samples",
        "number of samples (random states) on which to evaluate each "
        "candidate pattern collection",
        "1000",
        plugins::Bounds("1", "infinity"));
    feature.add_option<int>(
        "min_improvement",
        "minimum number of samples on which a candidate pattern "
        "collection must improve on the current one to be considered "
        "as the next pattern collection ",
        "10",
        plugins::Bounds("1", "infinity"));
    feature.add_option<double>(
        "max_time",
        "maximum time in seconds for improving the initial pattern "
        "collection via hill climbing. If set to 0, no hill climbing "
        "is performed at all. Note that this limit only affects hill "
        "climbing. Use max_time_dominance_pruning to limit the time "
        "spent for pruning dominated patterns.",
        "infinity",
        plugins::Bounds("0.0", "infinity"));

    add_pattern_collection_generator_options_to_feature(feature);
    utils::add_rng_options(feature);
}

void check_hillclimbing_options(
    const plugins::Options& opts,
    const utils::Context& context)
{
    if (opts.get<int>("min_improvement") > opts.get<int>("num_samples")) {
        context.error(
            "Minimum improvement must not be higher than number of samples");
    }
}

class PatternCollectionGeneratorHillclimbingFeature
    : public plugins::TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorHillclimbing> {
public:
    PatternCollectionGeneratorHillclimbingFeature()
        : TypedFeature("hillclimbing_probabilistic")
    {
        add_hillclimbing_options(*this);
    }

    std::shared_ptr<PatternCollectionGeneratorHillclimbing> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
    {
        check_hillclimbing_options(options, context);
        return std::make_shared<PatternCollectionGeneratorHillclimbing>(
            options);
    }
};

static plugins::FeaturePlugin<PatternCollectionGeneratorHillclimbingFeature>
    _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
