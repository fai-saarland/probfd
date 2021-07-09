#include "pattern_collection_generator_hillclimbing.h"

#include "canonical_pdbs_heuristic.h"
#include "incremental_canonical_pdbs.h"
#include "pattern_database.h"
#include "utils.h"
#include "validation.h"

#include "../option_parser.h"
#include "../plugin.h"

#include "../globals.h"
#include "../global_state.h"
#include "../state_registry.h"
#include "../causal_graph.h"
#include "../sampling.h"
#include "../utils/collections.h"
#include "../utils/countdown_timer.h"
#include "../utils/logging.h"
#include "../utils/markup.h"
#include "../utils/math.h"
#include "../utils/memory.h"
#include "../utils/rng.h"
#include "../utils/rng_options.h"
#include "../utils/timer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

using namespace std;
using utils::Verbosity;

namespace pdbs {
/* Since this exception class is only used for control flow and thus has no need
   for an error message, we use a standalone class instead of inheriting from
   utils::Exception. */
class HillClimbingTimeout {
};

static vector<int> get_goal_variables() {
    vector<int> goal_vars;
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
static vector<vector<int>> compute_relevant_neighbours() {
    const CausalGraph &causal_graph = *g_causal_graph;
    const vector<int> goal_vars = get_goal_variables();

    vector<vector<int>> connected_vars_by_variable;
    connected_vars_by_variable.reserve(g_variable_domain.size());
    for (int var_id = 0; var_id < static_cast<int>(g_variable_domain.size()); var_id++) {

        // Consider variables connected backwards via pre->eff arcs.
        const vector<int> &pre_to_eff_predecessors = causal_graph.get_eff_to_pre(var_id);

        // Consider goal variables connected (forwards) via eff--eff and pre->eff arcs.
        const vector<int> &causal_graph_successors = causal_graph.get_successors(var_id);
        vector<int> goal_variable_successors;
        set_intersection(
            causal_graph_successors.begin(), causal_graph_successors.end(),
            goal_vars.begin(), goal_vars.end(),
            back_inserter(goal_variable_successors));

        // Combine relevant goal and non-goal variables.
        vector<int> relevant_neighbours;
        set_union(
            pre_to_eff_predecessors.begin(), pre_to_eff_predecessors.end(),
            goal_variable_successors.begin(), goal_variable_successors.end(),
            back_inserter(relevant_neighbours));

        connected_vars_by_variable.push_back(move(relevant_neighbours));
    }
    return connected_vars_by_variable;
}

void PatternCollectionGeneratorHillclimbing::Statistics::
print(std::ostream& out) const {
    out << "\nHill Climbing Generator Statistics:"
        << "\n  Iterations: " << num_iterations
        << "\n  Generated patterns: " << generated_patterns
        << "\n  Rejected patterns: " << rejected_patterns
        << "\n  Maximum candidate PDB size: " << max_pdb_size
        << "\n  Time: " << hillclimbing_time
        << std::endl;
}

PatternCollectionGeneratorHillclimbing::PatternCollectionGeneratorHillclimbing(const Options &opts)
    : verbosity(static_cast<Verbosity>(opts.get_enum("verbosity"))),
      initial_generator(opts.get<std::shared_ptr<PatternCollectionGenerator>>(
          "initial_generator")),
      pdb_max_size(opts.get<int>("pdb_max_size")),
      collection_max_size(opts.get<int>("collection_max_size")),
      num_samples(opts.get<int>("num_samples")),
      min_improvement(opts.get<int>("min_improvement")),
      max_time(opts.get<double>("max_time")),
      rng(utils::parse_rng_from_options(opts)),
      num_rejected(0),
      hill_climbing_timer(0),
      cost_type(OperatorCost(opts.get_enum("cost_type"))) {
}

int PatternCollectionGeneratorHillclimbing::generate_candidate_pdbs(
    const vector<vector<int>> &relevant_neighbours,
    const PatternDatabase &pdb,
    set<Pattern> &generated_patterns,
    PDBCollection &candidate_pdbs) {
    const Pattern &pattern = pdb.get_pattern();
    int pdb_size = pdb.get_size();
    int max_pdb_size = 0;
    for (int pattern_var : pattern) {
        assert(utils::in_bounds(pattern_var, relevant_neighbours));
        const vector<int> &connected_vars = relevant_neighbours[pattern_var];

        // Only use variables which are not already in the pattern.
        vector<int> relevant_vars;
        set_difference(
            connected_vars.begin(), connected_vars.end(),
            pattern.begin(), pattern.end(),
            back_inserter(relevant_vars));

        for (int rel_var_id : relevant_vars) {
            int rel_var_size = g_variable_domain[rel_var_id];
            if (utils::is_product_within_limit(pdb_size, rel_var_size,
                                               pdb_max_size)) {
                Pattern new_pattern(pattern);
                new_pattern.push_back(rel_var_id);
                sort(new_pattern.begin(), new_pattern.end());
                if (!generated_patterns.count(new_pattern)) {
                    /*
                      If we haven't seen this pattern before, generate a PDB
                      for it and add it to candidate_pdbs if its size does not
                      surpass the size limit.
                    */
                    generated_patterns.insert(new_pattern);
                    candidate_pdbs.push_back(
                        make_shared<PatternDatabase>(new_pattern, cost_type));
                    max_pdb_size = max(max_pdb_size,
                                       candidate_pdbs.back()->get_size());
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
    int init_h,
    vector<GlobalState> &samples) {
    assert(samples.empty());

    samples.reserve(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        samples.push_back(sampler.sample_state(
                              init_h,
                              [this](const GlobalState &state) {
                                  return current_pdbs->is_dead_end(state);
                              }));
        if (hill_climbing_timer->is_expired()) {
            throw HillClimbingTimeout();
        }
    }
}

pair<int, int> PatternCollectionGeneratorHillclimbing::find_best_improving_pdb(
    const vector<GlobalState> &samples,
    const vector<int> &samples_h_values,
    PDBCollection &candidate_pdbs) {
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

        const shared_ptr<PatternDatabase> &pdb = candidate_pdbs[i];
        if (!pdb) {
            /* candidate pattern is too large or has already been added to
               the canonical heuristic. */
            continue;
        }
        /*
          If a candidate's size added to the current collection's size exceeds
          the maximum collection size, then forget the pdb.
        */
        int combined_size = current_pdbs->get_size() + pdb->get_size();
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
        vector<PatternClique> pattern_cliques =
            current_pdbs->get_pattern_cliques(pdb->get_pattern());
        for (int sample_id = 0; sample_id < num_samples; ++sample_id) {
            const GlobalState &sample = samples[sample_id];
            assert(utils::in_bounds(sample_id, samples_h_values));
            int h_collection = samples_h_values[sample_id];
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
        if (verbosity >= Verbosity::VERBOSE && count > 0) {
            cout << "pattern: " << candidate_pdbs[i]->get_pattern()
                 << " - improvement: " << count << endl;
        }
    }

    return make_pair(improvement, best_pdb_index);
}

bool PatternCollectionGeneratorHillclimbing::is_heuristic_improved(
    const PatternDatabase &pdb, const GlobalState &sample, int h_collection,
    const PDBCollection &pdbs, const vector<PatternClique> &pattern_cliques) {
    // h_pattern: h-value of the new pattern
    int h_pattern = pdb.get_value(sample);

    if (h_pattern == numeric_limits<int>::max()) {
        return true;
    }

    // h_collection: h-value of the current collection heuristic
    if (h_collection == numeric_limits<int>::max())
        return false;

    vector<int> h_values;
    h_values.reserve(pdbs.size());
    for (const shared_ptr<PatternDatabase> &p : pdbs) {
        int h = p->get_value(sample);
        if (h == numeric_limits<int>::max())
            return false;
        h_values.push_back(h);
    }
    for (const PatternClique &clilque : pattern_cliques) {
        int h_clique = 0;
        for (PatternID pattern_id : clilque) {
            h_clique += h_values[pattern_id];
        }
        if (h_pattern + h_clique > h_collection) {
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
    hill_climbing_timer = new utils::CountdownTimer(max_time);

    //cout << "Average operator cost: "
    //     << task_properties::get_average_operator_cost(task_proxy) << endl;

    const vector<vector<int>> relevant_neighbours =
        compute_relevant_neighbours();

    // Candidate patterns generated so far (used to avoid duplicates).
    set<Pattern> generated_patterns;
    // The PDBs for the patterns in generated_patterns that satisfy the size
    // limit to avoid recomputation.
    PDBCollection candidate_pdbs;
    // The maximum size over all PDBs in candidate_pdbs.
    int max_pdb_size = 0;
    for (const shared_ptr<PatternDatabase> &current_pdb :
         *(current_pdbs->get_pattern_databases())) {
        int new_max_pdb_size = generate_candidate_pdbs(
            relevant_neighbours, *current_pdb, generated_patterns,
            candidate_pdbs);
        max_pdb_size = max(max_pdb_size, new_max_pdb_size);
    }
    /*
      NOTE: The initial set of candidate patterns (in generated_patterns) is
      guaranteed to be "normalized" in the sense that there are no duplicates
      and patterns are sorted.
    */
    if (verbosity >= Verbosity::NORMAL) {
        cout << "Done calculating initial candidate PDBs" << endl;
    }

    int num_iterations = 0;
    StateRegistry state_registry;
    GlobalState initial_state = state_registry.get_initial_state();

    sampling::RandomWalkSampler sampler(state_registry, *rng);
    vector<GlobalState> samples;
    vector<int> samples_h_values;

    try {
        while (true) {
            ++num_iterations;
            int init_h = current_pdbs->get_value(initial_state);

            if (verbosity >= Verbosity::VERBOSE) {
                cout << "current collection size is "
                    << current_pdbs->get_size() << endl;
                cout << "current initial h value: ";
            }

            if (current_pdbs->is_dead_end(initial_state)) {
                if (verbosity >= Verbosity::VERBOSE) {
                    cout << "infinite => stopping hill climbing" << endl;
                }
                break;
            } else {
                if (verbosity >= Verbosity::VERBOSE) {
                    cout << init_h << endl;
                }
            }

            samples.clear();
            samples_h_values.clear();
            sample_states(sampler, init_h, samples);
            for (const GlobalState &sample : samples) {
                samples_h_values.push_back(current_pdbs->get_value(sample));
            }

            pair<int, int> improvement_and_index =
                find_best_improving_pdb(samples, samples_h_values, candidate_pdbs);
            int improvement = improvement_and_index.first;
            int best_pdb_index = improvement_and_index.second;

            if (improvement < min_improvement) {
                if (verbosity >= Verbosity::VERBOSE) {
                    cout << "Improvement below threshold. Stop hill climbing."
                        << endl;
                }
                break;
            }

            // Add the best PDB to the CanonicalPDBsHeuristic.
            assert(best_pdb_index != -1);
            const shared_ptr<PatternDatabase> &best_pdb =
                candidate_pdbs[best_pdb_index];
            const Pattern &best_pattern = best_pdb->get_pattern();

            if (verbosity >= Verbosity::VERBOSE) {
                cout << "found a better pattern with improvement " << improvement
                    << endl;
                cout << "pattern: " << best_pattern << endl;
            }

            current_pdbs->add_pdb(best_pdb);

            // Generate candidate patterns and PDBs for next iteration.
            int new_max_pdb_size = generate_candidate_pdbs(
                relevant_neighbours, *best_pdb, generated_patterns,
                candidate_pdbs);
            max_pdb_size = max(max_pdb_size, new_max_pdb_size);

            // Remove the added PDB from candidate_pdbs.
            candidate_pdbs[best_pdb_index] = nullptr;

            if (verbosity >= Verbosity::VERBOSE) {
                cout << "Hill climbing time so far: "
                    << hill_climbing_timer->get_elapsed_time()
                    << endl;
            }
        }
    } catch (HillClimbingTimeout &) {
        if (verbosity >= Verbosity::SILENT) {
            cout << "Time limit reached. Abort hill climbing." << endl;
        }
    }

    statistics_.reset(new Statistics(
        num_iterations,
        generated_patterns.size(),
        num_rejected,
        max_pdb_size,
        hill_climbing_timer->get_elapsed_time()));

    if (verbosity >= Verbosity::SILENT) {
        statistics_->print(std::cout);
    }

    delete hill_climbing_timer;
    hill_climbing_timer = nullptr;
}

PatternCollectionInformation PatternCollectionGeneratorHillclimbing::generate(OperatorCost cost_type) {
    this->cost_type = cost_type;

    utils::Timer timer;

    if (verbosity >= Verbosity::NORMAL) {
        cout << "Generating patterns using the hill climbing generator..." << endl;
    }

    // Generate initial collection
    assert (initial_generator);
    
    auto collection = initial_generator->generate(cost_type);
    current_pdbs = std::make_unique<IncrementalCanonicalPDBs>(
        cost_type, collection);

    if (verbosity >= Verbosity::NORMAL) {
        cout << "Done calculating initial pattern collection: " << timer << endl;
    }

    GlobalState initial_state = g_initial_state();
    if (!current_pdbs->is_dead_end(initial_state) && max_time > 0) {
        hill_climbing();
    }

    PatternCollectionInformation pci = current_pdbs->get_pattern_collection_information();

    if (verbosity >= Verbosity::VERBOSE) {
        dump_pattern_collection_generation_statistics(
            "Hill climbing generator", timer(), pci);
    }

    return pci;
}

std::shared_ptr<utils::Printable>
PatternCollectionGeneratorHillclimbing::get_report() const
{
    return statistics_;
}



void add_hillclimbing_options(OptionParser &parser) {
    utils::add_verbosity_option_to_parser(parser);

    parser.add_option<std::shared_ptr<PatternCollectionGenerator>>(
        "initial_generator",
        "generator for the initial pattern database ",
        "systematic(pattern_max_size=1)");
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

static shared_ptr<PatternCollectionGenerator> _parse(OptionParser &parser) {
    add_hillclimbing_options(parser);

    Options opts = parser.parse();
    if (parser.help_mode())
        return nullptr;

    check_hillclimbing_options(parser, opts);
    if (parser.dry_run())
        return nullptr;

    return make_shared<PatternCollectionGeneratorHillclimbing>(opts);
}

static shared_ptr<Heuristic> _parse_ipdb(OptionParser &parser) {
    parser.document_synopsis(
        "iPDB",
        "This pattern generation method is an adaption of the algorithm "
        "described in the following paper:" + utils::format_conference_reference(
            {"Patrik Haslum", "Adi Botea", "Malte Helmert", "Blai Bonet",
             "Sven Koenig"},
            "Domain-Independent Construction of Pattern Database Heuristics for"
            " Cost-Optimal Planning",
            "http://www.informatik.uni-freiburg.de/~ki/papers/haslum-etal-aaai07.pdf",
            "Proceedings of the 22nd AAAI Conference on Artificial"
            " Intelligence (AAAI 2007)",
            "1007-1012",
            "AAAI Press",
            "2007") +
        "For implementation notes, see:" + utils::format_conference_reference(
            {"Silvan Sievers", "Manuela Ortlieb", "Malte Helmert"},
            "Efficient Implementation of Pattern Database Heuristics for"
            " Classical Planning",
            "https://ai.dmi.unibas.ch/papers/sievers-et-al-socs2012.pdf",
            "Proceedings of the Fifth Annual Symposium on Combinatorial"
            " Search (SoCS 2012)",
            "105-111",
            "AAAI Press",
            "2012"));
    parser.document_note(
        "Note",
        "The pattern collection created by the algorithm will always contain "
        "all patterns consisting of a single goal variable, even if this "
        "violates the pdb_max_size or collection_max_size limits.");
    parser.document_language_support("action costs", "supported");
    parser.document_language_support("conditional effects", "not supported");
    parser.document_language_support("axioms", "not supported");
    parser.document_property("admissible", "yes");
    parser.document_property("consistent", "yes");
    parser.document_property("safe", "yes");
    parser.document_property("preferred operators", "no");
    parser.document_note(
        "Note",
        "This pattern generation method uses the canonical pattern collection "
        "heuristic.");
    parser.document_note(
        "Implementation Notes",
        "The following will very briefly describe the algorithm and explain "
        "the differences between the original implementation from 2007 and the "
        "new one in Fast Downward.\n\n"
        "The aim of the algorithm is to output a pattern collection for which "
        "the Evaluator#Canonical_PDB yields the best heuristic estimates.\n\n"
        "The algorithm is basically a local search (hill climbing) which "
        "searches the \"pattern neighbourhood\" (starting initially with a "
        "pattern for each goal variable) for improving the pattern collection. "
        "This is done as described in the section \"pattern construction as "
        "search\" in the paper, except for the corrected search "
        "neighbourhood discussed below. For evaluating the "
        "neighbourhood, the \"counting approximation\" as introduced in the "
        "paper was implemented. An important difference however consists in "
        "the fact that this implementation computes all pattern databases for "
        "each candidate pattern rather than using A* search to compute the "
        "heuristic values only for the sample states for each pattern.\n\n"
        "Also the logic for sampling the search space differs a bit from the "
        "original implementation. The original implementation uses a random "
        "walk of a length which is binomially distributed with the mean at the "
        "estimated solution depth (estimation is done with the current pattern "
        "collection heuristic). In the Fast Downward implementation, also a "
        "random walk is used, where the length is the estimation of the number "
        "of solution steps, which is calculated by dividing the current "
        "heuristic estimate for the initial state by the average operator "
        "costs of the planning task (calculated only once and not updated "
        "during sampling!) to take non-unit cost problems into account. This "
        "yields a random walk of an expected lenght of np = 2 * estimated "
        "number of solution steps. If the random walk gets stuck, it is being "
        "restarted from the initial state, exactly as described in the "
        "original paper.\n\n"
        "The section \"avoiding redundant evaluations\" describes how the "
        "search neighbourhood of patterns can be restricted to variables that "
        "are relevant to the variables already included in the pattern by "
        "analyzing causal graphs. There is a mistake in the paper that leads "
        "to some relevant neighbouring patterns being ignored. See the [errata "
        "https://ai.dmi.unibas.ch/research/publications.html] for details. This "
        "mistake has been addressed in this implementation. "
        "The second approach described in the paper (statistical confidence "
        "interval) is not applicable to this implementation, as it doesn't use "
        "A* search but constructs the entire pattern databases for all "
        "candidate patterns anyway.\n"
        "The search is ended if there is no more improvement (or the "
        "improvement is smaller than the minimal improvement which can be set "
        "as an option), however there is no limit of iterations of the local "
        "search. This is similar to the techniques used in the original "
        "implementation as described in the paper.",
        true);

    add_hillclimbing_options(parser);

    /*
      Add, possibly among others, the options for dominance pruning.

      Note that using dominance pruning during hill climbing could lead to fewer
      discovered patterns and pattern collections. A dominated pattern
      (or pattern collection) might no longer be dominated after more patterns
      are added. We thus only use dominance pruning on the resulting collection.
    */
    add_canonical_pdbs_options_to_parser(parser);

    Heuristic::add_options_to_parser(parser);

    Options opts = parser.parse();
    if (parser.help_mode())
        return nullptr;

    check_hillclimbing_options(parser, opts);

    if (parser.dry_run())
        return nullptr;

    shared_ptr<PatternCollectionGeneratorHillclimbing> pgh =
        make_shared<PatternCollectionGeneratorHillclimbing>(opts);

    Options heuristic_opts;
    heuristic_opts.set<int>("cost_type", opts.get_enum("cost_type"));
    heuristic_opts.set<shared_ptr<PatternCollectionGenerator>>(
        "patterns", pgh);
    heuristic_opts.set<double>(
        "max_time_dominance_pruning", opts.get<double>("max_time_dominance_pruning"));

    return make_shared<CanonicalPDBsHeuristic>(heuristic_opts);
}

static Plugin<Heuristic> _plugin_ipdb("ipdb", _parse_ipdb);
static Plugin<PatternCollectionGenerator> _plugin("hillclimbing", _parse);
}
