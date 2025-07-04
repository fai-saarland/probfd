#include "probfd/pdbs/pattern_collection_generator_hillclimbing.h"

#include "downward/axiom_space.h"
#include "downward/axioms.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/per_task_information.h"
#include "probfd/pdbs/pattern_collection_information.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/subcollection_finder_factory.h"
#include "probfd/pdbs/utils.h"

#include "probfd/cost_function.h"
#include "probfd/probabilistic_task.h"

#include "probfd/task_utils/causal_graph.h"
#include "probfd/task_utils/sampling.h"

#include "downward/algorithms/dynamic_bitset.h"

#include "downward/utils/collections.h"
#include "downward/utils/countdown_timer.h"
#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/timer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <utility>

using namespace downward;
using namespace downward::utils;

namespace probfd::pdbs {

static std::vector<int> get_goal_variables(const GoalFactList& goals)
{
    std::vector<int> goal_vars;
    goal_vars.reserve(goals.size());
    for (FactPair goal : goals) { goal_vars.push_back(goal.var); }
    assert(utils::is_sorted_unique(goal_vars));
    return goal_vars;
}

static unsigned long long compute_total_pdb_size(const PPDBCollection& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) { size += pdb->num_states(); }

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
static std::vector<std::vector<int>> compute_relevant_neighbours(
    const causal_graph::ProbabilisticCausalGraph& causal_graph,
    const VariableSpace& variables,
    const GoalFactList& goals)
{
    const std::vector<int> goal_vars = get_goal_variables(goals);

    std::vector<std::vector<int>> connected_vars_by_variable;
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
        std::ranges::set_intersection(
            causal_graph_successors,
            goal_vars,
            back_inserter(goal_variable_successors));

        // Combine relevant goal and non-goal variables.
        std::vector<int> relevant_neighbours;
        std::ranges::set_union(
            pre_to_eff_predecessors,
            goal_variable_successors,
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
    SharedProbabilisticTask task;

    PatternCollection patterns;
    PPDBCollection pattern_databases;
    std::vector<PatternSubCollection> pattern_subcollections;

    std::shared_ptr<SubCollectionFinder> subcollection_finder;

    heuristics::BlindHeuristic<StateRank> h;

    // The sum of all abstract state sizes of all pdbs in the collection.
    long long size;

    // Adds a PDB for pattern but does not recompute pattern_subcollections.
    void
    add_pdb_for_pattern(const Pattern& pattern, const State& initial_state);

    void recompute_pattern_subcollections();

public:
    IncrementalPPDBs(
        SharedProbabilisticTask task,
        PatternCollection initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder,
        const State& initial_state);

    IncrementalPPDBs(
        SharedProbabilisticTask task,
        PatternCollectionInformation& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    // Adds a new PDB to the collection and recomputes pattern_subcollections.
    void add_pdb(const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb);

    [[nodiscard]]
    int count_improvements(
        const ProbabilityAwarePatternDatabase& pdb,
        const std::vector<PatternCollectionGeneratorHillclimbing::Sample>&
            samples,
        value_t termination_cost) const;

    [[nodiscard]]
    value_t evaluate(
        const State& state,
        value_t cost_lower_bound,
        value_t termination_cost) const;

    /*
      The following method offers a quick dead-end check for the sampling
      procedure of iPDB-hillclimbing. This exists because we can much more
      efficiently test if the canonical heuristic is infinite than
      computing the exact heuristic value.
    */
    [[nodiscard]]
    bool is_dead_end(const State& state, value_t termination_cost) const;

    [[nodiscard]]
    PatternCollectionInformation get_pattern_collection_information() const;

    [[nodiscard]]
    const PPDBCollection& get_pattern_databases() const;

    [[nodiscard]]
    long long get_size() const;

private:
    [[nodiscard]]
    bool is_heuristic_improved(
        const ProbabilityAwarePatternDatabase& pdb,
        const PatternCollectionGeneratorHillclimbing::Sample& sample,
        const std::vector<PatternSubCollection>& pattern_subcollections,
        value_t termination_cost) const;
};

PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    SharedProbabilisticTask task,
    PatternCollection initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder,
    const State& initial_state)
    : task(std::move(task))
    , patterns(std::move(initial_patterns))
    , subcollection_finder(std::move(subcollection_finder))
    , h(get_operators(this->task),
        get_cost_function(this->task),
        get_termination_costs(this->task))
    , size(0)
{
    pattern_databases.reserve(patterns.size());
    for (const Pattern& pattern : patterns)
        add_pdb_for_pattern(pattern, initial_state);
    recompute_pattern_subcollections();
}

PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::IncrementalPPDBs(
    SharedProbabilisticTask task,
    PatternCollectionInformation& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : task(task)
    , patterns(initial_patterns.get_patterns())
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_subcollections(initial_patterns.get_subcollections())
    , subcollection_finder(std::move(subcollection_finder))
    , h(get_operators(this->task),
        get_cost_function(this->task),
        get_termination_costs(this->task))
    , size(compute_total_pdb_size(pattern_databases))
{
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    add_pdb_for_pattern(const Pattern& pattern, const State& initial_state)
{
    const auto& variables = get_variables(task);

    auto& pdb = pattern_databases.emplace_back(
        std::make_unique<ProbabilityAwarePatternDatabase>(variables, pattern));
    const StateRank abs_init = pdb->get_abstract_state(initial_state);
    compute_distances(*pdb, task, abs_init, h);
    size += pdb->num_states();
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::add_pdb(
    const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb)
{
    patterns.push_back(pdb->get_pattern());
    auto& new_pdb = pattern_databases.emplace_back(pdb);
    size += new_pdb->num_states();
    recompute_pattern_subcollections();
}

void PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    recompute_pattern_subcollections()
{
    pattern_subcollections =
        subcollection_finder->compute_subcollections(patterns);
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
            patterns,
            pattern_subcollections,
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
    value_t cost_lower_bound,
    value_t termination_cost) const
{
    return subcollection_finder->evaluate(
        pattern_databases,
        pattern_subcollections,
        state,
        cost_lower_bound,
        termination_cost);
}

bool PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::is_dead_end(
    const State& state,
    value_t termination_cost) const
{
    return std::ranges::any_of(pattern_databases, [=](const auto& pdb) {
        return pdb->lookup_estimate(state) == termination_cost;
    });
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::
    IncrementalPPDBs::get_pattern_collection_information() const
{
    PatternCollectionInformation result(task, patterns, subcollection_finder);
    result.set_pdbs(pattern_databases);
    result.set_subcollections(pattern_subcollections);
    return result;
}

const PPDBCollection& PatternCollectionGeneratorHillclimbing::IncrementalPPDBs::
    get_pattern_databases() const
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

    if (h_pattern == termination_cost) { return true; }

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

PatternCollectionGeneratorHillclimbing::PatternCollectionGeneratorHillclimbing(
    std::shared_ptr<PatternCollectionGenerator> initial_generator,
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory,
    int pdb_max_size,
    int collection_max_size,
    int num_samples,
    int min_improvement,
    double max_time,
    int search_space_max_size,
    std::shared_ptr<utils::RandomNumberGenerator> rng,
    utils::Verbosity verbosity)
    : PatternCollectionGenerator(verbosity)
    , initial_generator_(std::move(initial_generator))
    , subcollection_finder_factory_(std::move(subcollection_finder_factory))
    , pdb_max_size_(pdb_max_size)
    , collection_max_size_(collection_max_size)
    , num_samples_(num_samples)
    , min_improvement_(min_improvement)
    , max_time_(max_time)
    , rng_(std::move(rng))
    , remaining_states_(search_space_max_size)
    , num_rejected_(0)
{
}

PatternCollectionGeneratorHillclimbing::
    ~PatternCollectionGeneratorHillclimbing() = default;

unsigned int PatternCollectionGeneratorHillclimbing::generate_candidate_pdbs(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    utils::CountdownTimer& hill_climbing_timer,
    const std::vector<std::vector<int>>& relevant_neighbours,
    const ProbabilityAwarePatternDatabase& pdb,
    std::set<DynamicBitset>& generated_patterns,
    PPDBCollection& candidate_pdbs)
{
    using namespace utils;

    if (log_.is_at_least_verbose()) {
        std::cout << "Generating pattern neighborhood..." << std::endl;
    }

    const auto& variables = get_variables(task);

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
            const int max_size = std::min(pdb_max_size_, remaining_states_);

            if (!is_product_within_limit(pdb_size, rel_var_size, max_size)) {
                ++num_rejected_;

                if (log_.is_at_least_verbose()) {
                    std::cout << "Skipping neighboring pattern for variable "
                              << rel_var_id
                              << " because its PDB would exceed size limits."
                              << std::endl;
                }

                continue;
            }

            DynamicBitset bitset(variables.size());

            for (int var : pattern) { bitset.set(static_cast<size_t>(var)); }

            bitset.set(static_cast<size_t>(rel_var_id));

            if (!generated_patterns.insert(bitset).second) {
                if (log_.is_at_least_verbose()) {
                    std::cout << "Skipping neighboring pattern for variable "
                              << rel_var_id << " because it already exists."
                              << std::endl;
                }

                continue;
            }

            if (log_.is_at_least_verbose()) {
                std::cout
                    << "Generating neighboring PDB for pattern with variable "
                    << rel_var_id << std::endl;
            }

            /*
                If we haven't seen this pattern before, generate a PDB
                for it and add it to candidate_pdbs if its size does not
                surpass the size limit.
            */
            auto& new_pdb = candidate_pdbs.emplace_back(
                std::make_unique<ProbabilityAwarePatternDatabase>(
                    variables,
                    extended_pattern(pdb.get_pattern(), rel_var_id)));

            IncrementalPPDBEvaluator h(
                pdb.value_table,
                new_pdb->ranking_function,
                rel_var_id);

            const StateRank abs_init =
                new_pdb->get_abstract_state(initial_state);

            compute_distances(
                *new_pdb,
                task,
                abs_init,
                h,
                true,
                hill_climbing_timer.get_remaining_time());

            const unsigned int num_states = new_pdb->num_states();
            max_pdb_size = std::max(max_pdb_size, num_states);
            remaining_states_ -= num_states;
        }
    }

    return max_pdb_size;
}

void PatternCollectionGeneratorHillclimbing::sample_states(
    const State& initial_state,
    const utils::CountdownTimer& hill_climbing_timer,
    IncrementalPPDBs& current_pdbs,
    const sampling::RandomWalkSampler& sampler,
    value_t init_h,
    value_t cost_lower_bound,
    value_t termination_cost,
    std::vector<Sample>& samples) const
{
    assert(samples.empty());

    for (int i = 0; i < num_samples_; ++i) {
        auto f = [=, &current_pdbs](const State& state) {
            return current_pdbs.is_dead_end(state, termination_cost);
        };

        // TODO How to choose the length of the random walk in MaxProb?
        State sample = sampler.sample_state(init_h, initial_state, f);

        hill_climbing_timer.throw_if_expired();

        value_t h =
            current_pdbs.evaluate(sample, cost_lower_bound, termination_cost);
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
    value_t termination_cost) const
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
        if (combined_size > collection_max_size_) {
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

        if (log_.is_at_least_verbose() && count > 0) {
            std::cout << "pattern: " << candidate_pdbs[i]->get_pattern()
                      << " - improvement: " << count << std::endl;
        }
    }

    return std::make_pair(improvement, best_pdb_index);
}

void PatternCollectionGeneratorHillclimbing::hill_climbing(
    const SharedProbabilisticTask& task,
    const State& initial_state,
    IncrementalPPDBs& current_pdbs,
    value_t cost_lower_bound)
{
    const auto& variables = get_variables(task);
    const auto& axioms = get_axioms(task);
    const auto& operators = get_operators(task);
    const auto& goals = get_goal(task);
    const auto& cost_function = get_cost_function(task);
    const auto& term_costs = get_termination_costs(task);

    const auto& cg =
        causal_graph::get_causal_graph(variables, axioms, operators);

    auto& axiom_evaluator = g_axiom_evaluators[variables, axioms];

    const value_t termination_cost = term_costs.get_non_goal_termination_cost();

    int num_iterations = 0;
    utils::CountdownTimer hill_climbing_timer(max_time_);

    const PatternCollection relevant_neighbours =
        compute_relevant_neighbours(cg, variables, goals);

    // Candidate patterns generated so far (used to avoid duplicates).
    std::set<DynamicBitset> generated_patterns;
    // The PDBs for the patterns in generated_patterns that satisfy the size
    // limit to avoid recomputation.
    PPDBCollection candidate_pdbs;
    // The maximum size over all PDBs in candidate_pdbs.
    unsigned int max_pdb_size = 0;
    const int max_search_space_size = remaining_states_;

    try {
        for (const auto& current_pdb : current_pdbs.get_pattern_databases()) {
            unsigned int new_max_pdb_size = generate_candidate_pdbs(
                task,
                initial_state,
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
        if (log_.is_at_least_normal()) {
            std::cout << "Done calculating initial candidate PDBs" << std::endl;
        }

        sampling::RandomWalkSampler sampler(
            variables,
            operators,
            cost_function,
            axiom_evaluator,
            *rng_);

        std::vector<Sample> samples;
        samples.reserve(num_samples_);

        while (true) {
            ++num_iterations;
            value_t init_h = current_pdbs.evaluate(
                initial_state,
                cost_lower_bound,
                termination_cost);
            const bool initial_dead = init_h == termination_cost;

            if (log_.is_at_least_verbose()) {
                std::cout << "current collection size is "
                          << current_pdbs.get_size() << "\n"
                          << "current search space size is "
                          << max_search_space_size - remaining_states_
                          << "\ncurrent initial h value: ";

                if (initial_dead) {
                    std::cout << "infinite => stopping hill climbing"
                              << std::endl;
                } else {
                    std::cout << init_h << std::endl;
                }
            }

            if (initial_dead) { break; }

            sample_states(
                initial_state,
                hill_climbing_timer,
                current_pdbs,
                sampler,
                init_h,
                cost_lower_bound,
                termination_cost,
                samples);

            const auto [improvement, best_pdb_index] = find_best_improving_pdb(
                hill_climbing_timer,
                current_pdbs,
                samples,
                candidate_pdbs,
                termination_cost);

            samples.clear();

            if (improvement < min_improvement_) {
                if (log_.is_at_least_verbose()) {
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

            if (log_.is_at_least_verbose()) {
                std::cout << "found a better pattern with improvement "
                          << improvement << std::endl;
                std::cout << "pattern: " << best_pattern << std::endl;
            }

            current_pdbs.add_pdb(best_pdb);

            // Generate candidate patterns and PDBs for next iteration.
            unsigned int new_max_pdb_size = generate_candidate_pdbs(
                task,
                initial_state,
                hill_climbing_timer,
                relevant_neighbours,
                *best_pdb,
                generated_patterns,
                candidate_pdbs);

            max_pdb_size = std::max(max_pdb_size, new_max_pdb_size);

            // Remove the added PDB from candidate_pdbs.
            candidate_pdbs[best_pdb_index] = nullptr;

            if (log_.is_at_least_verbose()) {
                std::cout << "Hill climbing time so far: "
                          << hill_climbing_timer.get_elapsed_time()
                          << std::endl;
            }
        }
    } catch (utils::TimeoutException&) {
        if (log_.is_at_least_normal()) {
            std::cout << "Time limit reached. Abort hill climbing."
                      << std::endl;
        }
    }

    if (log_.is_at_least_normal()) {
        log_ << "\n"
             << "Hill Climbing Generator Statistics:" << "\n  Iterations: "
             << num_iterations
             << "\n  Generated patterns: " << generated_patterns.size()
             << "\n  Rejected patterns: " << num_rejected_
             << "\n  Maximum candidate PDB size: " << max_pdb_size
             << "\n  Time: " << hill_climbing_timer.get_elapsed_time() << "s"
             << std::endl;
    }
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::generate(
    const SharedProbabilisticTask& task)
{
    utils::Timer timer;

    if (log_.is_at_least_normal()) {
        std::cout << "Generating patterns using the hill climbing generator..."
                  << std::endl;
    }

    // Generate initial collection
    assert(initial_generator_);

    auto collection = initial_generator_->generate(task);
    std::shared_ptr<SubCollectionFinder> subcollection_finder =
        subcollection_finder_factory_->create_subcollection_finder(task);

    IncrementalPPDBs current_pdbs(task, collection, subcollection_finder);

    if (log_.is_at_least_normal()) {
        std::cout << "Done calculating initial pattern collection: " << timer
                  << std::endl;
    }

    const auto& operators = get_operators(task);
    const auto& cost_function = get_cost_function(task);
    const auto& init_vals = get_init(task);
    const auto& term_costs = get_termination_costs(task);

    const State initial_state = init_vals.get_initial_state();
    initial_state.unpack();

    const value_t cost_lower_bound = task_properties::get_cost_lower_bound(
        operators,
        cost_function,
        term_costs);
    const value_t termination_cost = term_costs.get_non_goal_termination_cost();

    value_t init_h = current_pdbs.evaluate(
        initial_state,
        cost_lower_bound,
        termination_cost);

    if (init_h != termination_cost && max_time_ > 0) {
        hill_climbing(task, initial_state, current_pdbs, cost_lower_bound);
    }

    PatternCollectionInformation pci =
        current_pdbs.get_pattern_collection_information();

    return pci;
}

} // namespace probfd::pdbs
