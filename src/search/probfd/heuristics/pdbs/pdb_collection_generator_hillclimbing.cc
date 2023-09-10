#include "probfd/heuristics/pdbs/pdb_collection_generator_hillclimbing.h"

#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"
#include "probfd/heuristics/pdbs/subcollection_finder_factory.h"
#include "probfd/heuristics/pdbs/utils.h"

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
#include <concepts>
#include <iostream>
#include <limits>
#include <ranges>

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

struct PDBCollectionGeneratorHillclimbing::Sample {
    State state;
    value_t h;
};

class PDBCollectionGeneratorHillclimbing::IncrementalPPDBs {
    ProbabilisticTaskProxy task_proxy;
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function;

    PPDBCollection pattern_databases;
    std::vector<PatternSubCollection> pattern_subcollections;

    std::shared_ptr<SubCollectionFinder> subcollection_finder;

    // The sum of all abstract state sizes of all pdbs in the collection.
    long long size;

    // Adds a PDB for pattern but does not recompute pattern_subcollections.
    void add_pdb_for_pattern(const Pattern& pattern, const State& intial_state);

    void recompute_pattern_subcollections();

public:
    IncrementalPPDBs(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        const PatternCollection& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    IncrementalPPDBs(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        PDBCollectionInformation& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    // Adds a new PDB to the collection and recomputes pattern_subcollections.
    ProbabilityAwarePatternDatabase&
    add_pdb(std::shared_ptr<ProbabilityAwarePatternDatabase> pdb);

    int count_improvements(
        const ProbabilityAwarePatternDatabase& pdb,
        const std::vector<PDBCollectionGeneratorHillclimbing::Sample>& samples,
        value_t termination_cost) const;

    value_t evaluate(const State& state, value_t termination_cost) const;

    /*
      The following method offers a quick dead-end check for the sampling
      procedure of iPDB-hillclimbing. This exists because we can much more
      efficiently test if the canonical heuristic is infinite than
      computing the exact heuristic value.
    */
    bool is_dead_end(const State& state, value_t termination_cost) const;

    PDBCollectionInformation get_pdb_collection_information() const;

    const PPDBCollection& get_pattern_databases() const;

    long long get_size() const;

private:
    bool is_heuristic_improved(
        const ProbabilityAwarePatternDatabase& pdb,
        const PDBCollectionGeneratorHillclimbing::Sample& sample,
        const std::vector<PatternSubCollection>& pattern_subcollections,
        value_t termination_cost) const;
};

PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    const PatternCollection& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(std::move(task_cost_function))
    , subcollection_finder(subcollection_finder)
    , size(0)
{
    pattern_databases.reserve(initial_patterns.size());
    for (const Pattern& pattern : initial_patterns)
        add_pdb_for_pattern(pattern, task_proxy.get_initial_state());
    recompute_pattern_subcollections();
}

PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    PDBCollectionInformation& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task_proxy(task_proxy)
    , task_cost_function(std::move(task_cost_function))
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_subcollections(initial_patterns.get_subcollections())
    , subcollection_finder(subcollection_finder)
    , size(compute_total_pdb_size(pattern_databases))
{
}

void PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::add_pdb_for_pattern(
    const Pattern& pattern,
    const State& initial_state)
{
    auto& pdb =
        pattern_databases.emplace_back(new ProbabilityAwarePatternDatabase(
            task_proxy,
            *task_cost_function,
            pattern,
            initial_state));
    size += pdb->num_states();
}

ProbabilityAwarePatternDatabase&
PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::add_pdb(
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb)
{
    auto& new_pdb = pattern_databases.emplace_back(std::move(pdb));
    size += new_pdb->num_states();
    recompute_pattern_subcollections();
    return *new_pdb;
}

void PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::
    recompute_pattern_subcollections()
{
    pattern_subcollections =
        subcollection_finder->compute_subcollections(pattern_databases);
}

int PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::count_improvements(
    const ProbabilityAwarePatternDatabase& pdb,
    const std::vector<Sample>& samples,
    value_t termination_cost) const
{
    int count = 0;
    std::vector<PatternSubCollection> subcollections =
        subcollection_finder->compute_subcollections_with_pdbs(
            pattern_databases,
            pattern_subcollections,
            pdb);
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

value_t PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::evaluate(
    const State& state,
    value_t termination_cost) const
{
    return subcollection_finder->evaluate(
        pattern_databases,
        pattern_subcollections,
        state,
        termination_cost);
}

bool PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::is_dead_end(
    const State& state,
    value_t termination_cost) const
{
    for (const auto& pdb : pattern_databases) {
        if (pdb->lookup_estimate(state) == termination_cost) {
            return true;
        }
    }

    return false;
}

PDBCollectionInformation PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::
    get_pdb_collection_information() const
{
    return PDBCollectionInformation(
        std::move(pattern_databases),
        std::move(subcollection_finder),
        std::move(pattern_subcollections));
}

const PPDBCollection&
PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::get_pattern_databases()
    const
{
    return pattern_databases;
}

long long PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::get_size() const
{
    return size;
}

bool PDBCollectionGeneratorHillclimbing::IncrementalPPDBs::
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
    h_values.reserve(pattern_databases.size());

    for (const auto& p : pattern_databases) {
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

PDBCollectionGeneratorHillclimbing::PDBCollectionGeneratorHillclimbing(
    const plugins::Options& opts)
    : PDBCollectionGenerator(opts)
    , initial_generator(opts.get<std::shared_ptr<PDBCollectionGenerator>>(
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

PDBCollectionGeneratorHillclimbing::~PDBCollectionGeneratorHillclimbing() =
    default;

unsigned int PDBCollectionGeneratorHillclimbing::generate_candidate_pdbs(
    ProbabilisticTaskProxy task_proxy,
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
                    task_cost_function,
                    extended_pattern(pdb.get_pattern(), rel_var_id),
                    task_proxy.get_initial_state(),
                    pdb,
                    true,
                    hill_climbing_timer.get_remaining_time()));
            const unsigned int num_states = new_pdb->num_states();
            max_pdb_size = std::max(max_pdb_size, num_states);
            remaining_states -= num_states;
        }
    }

    return max_pdb_size;
}

void PDBCollectionGeneratorHillclimbing::sample_states(
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

using namespace std::ranges;

template <class InputIt, class UnaryPredicate, class UnaryPredicate2>
constexpr InputIt find_if_else_do(
    InputIt first,
    InputIt last,
    UnaryPredicate p,
    UnaryPredicate2 p2)
{
    for (; first != last; ++first)
        if (p(*first))
            return first;
        else
            p2(*first);

    return last;
}

template <input_range Range>
auto remove_if_else_do(
    Range&& R,
    std::predicate<range_reference_t<std::remove_reference_t<Range>>> auto p,
    std::invocable<range_reference_t<std::remove_reference_t<Range>>> auto p2)
{
    auto first = std::begin(R);
    auto last = std::end(R);
    first = find_if_else_do(first, last, p, p2);
    if (first != last)
        for (auto i = first; ++i != last;)
            if (!p(*i)) p2(*first++ = std::move(*i));
    return first;
}

std::pair<int, std::shared_ptr<ProbabilityAwarePatternDatabase>*>
PDBCollectionGeneratorHillclimbing::find_best_improving_pdb(
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
    std::shared_ptr<ProbabilityAwarePatternDatabase>* best_pdb = nullptr;

    // Iterate over all candidates and search for the best improving pattern/pdb
    // Remove pdbs that exceed the size limit on the fly
    auto it = remove_if_else_do(
        candidate_pdbs,
        [&](auto& pdb) {
            assert(pdb);
            /*
              If a candidate's size added to the current collection's size
              exceeds the maximum collection size, then forget the pdb.
            */
            return current_pdbs.get_size() + pdb->num_states() >
                   collection_max_size;
        },
        [&](auto& pdb) {
            hill_climbing_timer.throw_if_expired();

            /*
              Calculate the "counting approximation" for all sample states:
              count the number of samples for which the current pattern
              collection heuristic would be improved if the new pattern was
              included into it.
            */
            const int count = current_pdbs.count_improvements(
                *pdb,
                samples,
                termination_cost);

            if (count > improvement) {
                improvement = count;
                best_pdb = &pdb;
            }

            if (log.is_at_least_verbose() && count > 0) {
                std::cout << "pattern: " << pdb->get_pattern()
                          << " - improvement: " << count << std::endl;
            }
        });

    candidate_pdbs.erase(it, candidate_pdbs.end());

    return std::make_pair(improvement, best_pdb);
}

void PDBCollectionGeneratorHillclimbing::hill_climbing(
    const ProbabilisticTask* task,
    ProbabilisticTaskProxy task_proxy,
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
        for (const auto& current_pdb : current_pdbs.get_pattern_databases()) {
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

            const auto [improvement, best_pdb_addr] = find_best_improving_pdb(
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
            assert(best_pdb_addr);
            const Pattern& best_pattern = (*best_pdb_addr)->get_pattern();

            if (log.is_at_least_verbose()) {
                std::cout << "found a better pattern with improvement "
                          << improvement << std::endl;
                std::cout << "pattern: " << best_pattern << std::endl;
            }

            auto& best_pdb = current_pdbs.add_pdb(std::move(*best_pdb_addr));

            // Remove the added PDB from candidate_pdbs.
            if (best_pdb_addr != &candidate_pdbs.back()) {
                *best_pdb_addr = std::move(candidate_pdbs.back());
            }

            candidate_pdbs.pop_back();

            // Generate candidate patterns and PDBs for next iteration.
            unsigned int new_max_pdb_size = generate_candidate_pdbs(
                task_proxy,
                task_cost_function,
                hill_climbing_timer,
                relevant_neighbours,
                best_pdb,
                generated_patterns,
                candidate_pdbs);

            max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);

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
        log << "\nHill Climbing Generator Statistics:"
            << "\n  Iterations: " << num_iterations
            << "\n  Generated patterns: " << generated_patterns.size()
            << "\n  Rejected patterns: " << num_rejected
            << "\n  Maximum candidate PDB size: " << max_pdb_size
            << "\n  Time: " << hill_climbing_timer.get_elapsed_time() << "s"
            << std::endl;
    }
}

PDBCollectionInformation PDBCollectionGeneratorHillclimbing::generate(
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

    PDBCollectionInformation pci =
        current_pdbs.get_pdb_collection_information();

    return pci;
}

void add_hillclimbing_options(plugins::Feature& feature)
{
    feature.add_option<std::shared_ptr<PDBCollectionGenerator>>(
        "initial_generator",
        "generator for the initial pattern database ",
        "classical_generator(generator=systematic(pattern_max_size=1))");

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

    add_pdb_collection_generator_options_to_feature(feature);
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

class PDBCollectionGeneratorHillclimbingFeature
    : public plugins::TypedFeature<
          PDBCollectionGenerator,
          PDBCollectionGeneratorHillclimbing> {
public:
    PDBCollectionGeneratorHillclimbingFeature()
        : TypedFeature("hillclimbing_probabilistic")
    {
        add_hillclimbing_options(*this);
    }

    std::shared_ptr<PDBCollectionGeneratorHillclimbing> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
    {
        check_hillclimbing_options(options, context);
        return std::make_shared<PDBCollectionGeneratorHillclimbing>(options);
    }
};

static plugins::FeaturePlugin<PDBCollectionGeneratorHillclimbingFeature>
    _plugin;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
