#include "legacy/merge_and_shrink/merge_and_shrink_heuristic.h"

#include "legacy/merge_and_shrink/abstraction.h"
#include "legacy/merge_and_shrink/linear_merge_strategy.h"
#include "legacy/merge_and_shrink/shrink_bisimulation.h"
#include "legacy/merge_and_shrink/shrink_fh.h"
#include "legacy/merge_and_shrink/shrink_label_subset_bisimulation.h"
#include "legacy/merge_and_shrink/variable_order_finder.h"

#include "legacy/global_state.h"
#include "legacy/globals.h"

#include "utils/timer.h"

#include "task_utils/causal_graph.h"

#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <limits>
#include <vector>
#ifdef DEBUG_ABSTRACTIONS
#include <fstream>
#include <string>
#endif
using namespace std;

namespace legacy {
namespace merge_and_shrink {

MergeAndShrinkHeuristic::MergeAndShrinkHeuristic(const options::Options& opts)
    : Heuristic(opts)
    , abstraction_count(opts.get<int>("count"))
    , merge_criteria(
          opts.get_list<std::shared_ptr<MergeCriterion>>("merge_criteria"))
    , merge_order(opts.get<MergeOrder>("merge_order"))
    , merge_strategy(opts.get<MergeStrategy>("merge_strategy"))
    , shrink_strategy(
          opts.get<std::shared_ptr<ShrinkStrategy>>("shrink_strategy"))
    , use_label_reduction(opts.get<bool>("reduce_labels"))
    , prune_unreachable(opts.get<bool>("prune_unreachable"))
    , prune_irrelevant(opts.get<bool>("prune_irrelevant"))
    , use_expensive_statistics(opts.get<bool>("expensive_statistics"))
    , use_label_inheritance(opts.get<bool>("label_inheritance"))
    , new_approach(opts.get<bool>("new_approach"))
    , use_uniform_distances(opts.get<bool>("use_uniform_distances"))
    , check_unsolvability(opts.get<bool>("check_unsolvability"))
    , continue_if_unsolvable(opts.get<bool>("continue_if_unsolvable"))
    , merge_size_limit(opts.get<int>("merge_size_limit"))
    , max_branching_merge(opts.get<int>("max_branching_merge"))
    , c_release_memory(
          opts.contains("release_memory") ? opts.get<bool>("release_memory")
                                          : true)
{
    bool use_empty_label_shrinking =
        opts.get<bool>("use_empty_label_shrinking");
    if (use_empty_label_shrinking) {
        empty_label_shrink_strategy = ShrinkEmptyLabels::create_default();
    } else {
        empty_label_shrink_strategy = 0;
    }
}

MergeAndShrinkHeuristic::~MergeAndShrinkHeuristic()
{
}

void MergeAndShrinkHeuristic::dump_options() const
{
    cout << "MergeStrategy: ";
    switch (merge_strategy) {
    case MERGE_LINEAR_MULTIPLE_DFS: cout << "Multiple:"; break; // TODO correct?
    case MERGE_LINEAR:
        for (size_t i = 0; i < merge_criteria.size(); i++) {
            cout << merge_criteria[i]->get_name() << "_";
        }
        switch (merge_order) {
        case LEVEL: cout << "LEVEL"; break;
        case REVERSE_LEVEL: cout << "REVERSE_LEVEL"; break;
        case RANDOM: cout << "RANDOM"; break;
        default: cerr << "Unknown merge criterion."; abort();
        }
        break;
    case MERGE_LINEAR_CG_GOAL_LEVEL:
        cout << "linear CG/GOAL, tie breaking on level (main)";
        break;
    case MERGE_LINEAR_CG_GOAL_RANDOM:
        cout << "linear CG/GOAL, tie breaking random";
        break;
    case MERGE_LINEAR_GOAL_CG_LEVEL:
        cout << "linear GOAL/CG, tie breaking on level";
        break;
    case MERGE_LINEAR_RANDOM: cout << "linear random"; break;
    case MERGE_DFP:
        cout << "Draeger/Finkbeiner/Podelski" << endl;
        cerr << "DFP merge strategy not implemented." << endl;
        exit(-1);
        break;
    case MERGE_LINEAR_LEVEL: cout << "linear by level"; break;
    case MERGE_LINEAR_REVERSE_LEVEL: cout << "linear by reverse level"; break;
    default: cerr << "Unknown merge strategy." << endl; exit(-1);
    }
    cout << endl;

    shrink_strategy->dump_options();
    if (empty_label_shrink_strategy) {
        cout << "Also using empty label shrinking" << endl;
        empty_label_shrink_strategy->dump_options();
    }
    cout << "Number of abstractions to maximize over: " << abstraction_count
         << endl;
    cout << "Label reduction: "
         << (use_label_reduction ? "enabled" : "disabled") << endl
         << "Prune unreachable states: " << (prune_unreachable ? "yes" : "no")
         << endl
         << "Prune irrelevant states: " << (prune_irrelevant ? "yes" : "no")
         << endl
         << "Expensive statistics: "
         << (use_expensive_statistics ? "enabled" : "disabled") << endl;
    if (use_label_inheritance) {
        cout << "Using the " << (new_approach ? "new" : "old")
             << " label inheritance implementation" << endl;
    } else {
        cout << "Using no label inheritance" << endl;
    }

    cout << "Merge size limit: " << merge_size_limit << endl;
    cout << "Max branching merge: " << max_branching_merge << endl;
}

void MergeAndShrinkHeuristic::warn_on_unusual_options() const
{
    if (use_expensive_statistics) {
        string dashes(79, '=');
        cerr << dashes << endl
             << ("WARNING! You have enabled extra statistics for "
                 "merge-and-shrink heuristics.\n"
                 "These statistics require a lot of time and memory.\n"
                 "When last tested (around revision 3011), enabling the "
                 "extra statistics\nincreased heuristic generation time by "
                 "76%. This figure may be significantly\nworse with more "
                 "recent code or for particular domains and instances.\n"
                 "You have been warned. Don't use this for benchmarking!")
             << endl
             << dashes << endl;
    }

    if (empty_label_shrink_strategy) {
        assert(check_unsolvability == true);
        if (!check_unsolvability) {
            string dashes(79, '=');
            cerr << dashes << endl
                 << ("WARNING! Empty label shrinking is activated and the "
                     "check for "
                     "unsolvability is desactivated!")
                 << endl
                 << dashes << endl;
        }
    }
}

// Filter irrelevant variables according to two criteria:
//   a) Skip variables already in the abstraction
//   b) Skip irrelevant variables
//   c) Skip merging variables whose size is surpassed
//   d) Skip combinations already visited
void MergeAndShrinkHeuristic::get_candidate_vars(
    vector<int>& res,
    set<set<int>>& explored_varsets,
    Abstraction* abstraction) const
{
    set<int> vset;
    if (abstraction) {
        const std::vector<int>& varsabs = abstraction->get_varset();
        vset.insert(varsabs.begin(), varsabs.end());
        explored_varsets.insert(vset);
    }

    for (size_t i = 0; i < g_variable_domain.size(); ++i) {
        // a) Skip variables already in the abstraction
        if (vset.count(i)) {
            continue;
        }

        //  b) Skip irrelevant variables
        if (atomic_abstractions[i]->is_irrelevant()) {
            continue;
        }

        // c) Skip merging variables whose size is surpassed
        if (abstraction &&
            abstraction->size() * atomic_abstractions[i]->size() >
                merge_size_limit) {
            // cout << "Discarded by size" << endl;
            continue;
        }

        // d) Skip combinations already visited
        vset.insert(i);
        if (explored_varsets.count(vset)) {
            vset.erase(i);
            continue;
        }
        vset.erase(i);
        res.push_back(i);
    }
}

/* We have two different methods in charge of preprocessing
 abstractions: normalize, compute distances, etc:
 prepare_before_merge, and prepare_after_merge:

 prepare_before_merge:
 prepare_after_merge:
*/

void MergeAndShrinkHeuristic::prepare_after_merge(
    Abstraction* abstraction,
    bool force_reduce_labels) const
{
    // TODO: When using nonlinear merge strategies, make sure not
    // to normalize multiple parts of a composite. See issue68.
    if (shrink_strategy->reduce_labels_before_shrinking() ||
        force_reduce_labels || use_label_inheritance ||
        empty_label_shrink_strategy) {
        abstraction->normalize(
            use_label_reduction,
            shrink_strategy->get_caught_labels_set(),
            use_label_inheritance,
            new_approach);
        // Note: label inheritance for atomic abstractions works only for linear
        // merging strategies, as the label set cannot be empty for the MAIN
        // abstraction
        //     other_abstraction->normalize(false,
        //     shrink_strategy->get_caught_labels_set(),
        // 				 use_label_inheritance, new_approach);
    }

    if (empty_label_shrink_strategy) {
        cout << "applying empty label shrinking" << endl;
        // HACK, we need two abstractions for shrink_before_merge, so instead we
        // call shrink_atomic (it does the same in the empty label shrinking)
        // empty_label_shrink_strategy->shrink_before_merge(*abstraction,
        // *other_abstraction);
        empty_label_shrink_strategy->shrink_atomic(*abstraction);
        // TODO: When using nonlinear merge strategies, make sure not
        // to normalize multiple parts of a composite. See issue68.
        if (shrink_strategy->reduce_labels_before_shrinking() ||
            use_label_inheritance) {
            abstraction->normalize(
                false /*use_label_reduction*/ /*no need to reduce labels again
                                                 -- transitions with same labels
                                                 are already merged!*/
                ,
                shrink_strategy->get_caught_labels_set(),
                use_label_inheritance,
                new_approach);
            // other_abstraction->normalize(false,
            // shrink_strategy->get_caught_labels_set(), use_label_inheritance,
            // new_approach);
        } else {
            /* just to keep the statistics up to date */
            // Peter: number of states and/or transitions cannot have increased,
            // so no need to update statistics here
            // abstraction->statistics(use_expensive_statistics);
            // other_abstraction->statistics(use_expensive_statistics);
        }
    }

    abstraction->compute_distances(
        prune_unreachable,
        prune_irrelevant,
        continue_if_unsolvable,
        use_uniform_distances);
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // abstraction->statistics(use_expensive_statistics);

#ifdef DEBUG_ABSTRACTIONS
    // Dumping the abstract state space into dot file
    ostringstream out;
    out << "abstract.space"
        << "." << abs_count << ".dot";
    ofstream myfile;
    myfile.open(out.str().c_str(), ios::out);
    if (myfile.is_open()) {
        abstraction->dump(myfile);
        myfile.close();
    } else {
        cout << "Unable to open dot file!" << endl;
        exit(1);
    }
#endif
}

void MergeAndShrinkHeuristic::prepare_before_merge(
    Abstraction* abstraction,
    Abstraction* other_abstraction) const
{
    if (shrink_strategy->reduce_labels_before_shrinking() ||
        use_label_inheritance || empty_label_shrink_strategy) {
        // Note: label inheritance for atomic abstractions works only for linear
        // merging strategies, as the label set cannot be empty for the MAIN
        // abstraction
        other_abstraction->normalize(
            false,
            shrink_strategy->get_caught_labels_set(),
            use_label_inheritance,
            new_approach);
    }
    if (empty_label_shrink_strategy) {
        cout << "applying empty label shrinking" << endl;
        empty_label_shrink_strategy->shrink_before_merge(
            *abstraction,
            *other_abstraction);
        // TODO: When using nonlinear merge strategies, make sure not
        // to normalize multiple parts of a composite. See issue68.
        if (shrink_strategy->reduce_labels_before_shrinking() ||
            use_label_inheritance) {
            other_abstraction->normalize(
                false,
                shrink_strategy->get_caught_labels_set(),
                use_label_inheritance,
                new_approach);
        }
    }

    other_abstraction->compute_distances(
        prune_unreachable,
        prune_irrelevant,
        continue_if_unsolvable,
        use_uniform_distances);
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // other_abstraction->statistics(use_expensive_statistics);
#ifdef DEBUG_ABSTRACTIONS
    cout << "DUMPING abstraction BEFORE SHRINKING" << endl;
    abstraction->dump();
    cout << "DUMPING other_abstraction BEFORE SHRINKING" << endl;
    other_abstraction->dump();
#endif
    shrink_strategy->shrink_before_merge(*abstraction, *other_abstraction);

#ifdef DEBUG_ABSTRACTIONS
    // Commented out since we shouldn't usually need/want this, even when
    // debugging. It will also give us warnings since the abstraction is not
    // normalized here.
    /*
      cout << "DUMPING abstraction AFTER SHRINKING" << endl;
      abstraction->dump();
      cout << "DUMPING other_abstraction AFTER SHRINKING" << endl;
      other_abstraction->dump();
    */

    ostringstream out2;
    out2 << "abstract.space.after.shrink"
         << "." << abs_count++ << ".dot";
    ofstream myfile2;
    myfile2.open(out2.str().c_str(), ios::out);
    if (myfile2.is_open()) {
        abstraction->dump(myfile2);
        myfile2.close();
    } else {
        cout << "Unable to open dot file!" << endl;
        exit(1);
    }
#endif

    // TODO: Make shrink_before_merge return a pair of bools
    //       that tells us whether they have actually changed,
    //       and use that to decide whether to dump statistics?
    // (The old code would print statistics on abstraction iff it was
    // shrunk. This is not so easy any more since this method is not
    // in control, and the shrink strategy doesn't know whether we want
    // expensive statistics. As a temporary aid, we just print the
    // statistics always now, whether or not we shrunk.)
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // abstraction->statistics(use_expensive_statistics);
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // other_abstraction->statistics(use_expensive_statistics);

    /* apply label inheritance only once, i.e., before the shrinking step */
    /* PETER: Why do we use label reduction again -- should the transitions
     * with equal labels not already be merged, at least if
     * shrink-strategy->reduce_labels_before_shrinking() returns true?! */
    abstraction->normalize(
        use_label_reduction,
        shrink_strategy->get_caught_labels_set(),
        false);
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // abstraction->statistics(use_expensive_statistics);

    // Don't label-reduce the atomic abstraction -- see issue68.
    other_abstraction->normalize(
        false,
        shrink_strategy->get_caught_labels_set(),
        false);
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // other_abstraction->statistics(use_expensive_statistics);
}

// As build abstraction, but calling to the dfs procedure instead
void MergeAndShrinkHeuristic::build_multiple_abstractions(
    vector<Abstraction*>& res_abstractions)
{
    build_atomic_abstractions();
    for (size_t i = 0; i < atomic_abstractions.size(); ++i) {
        if (!atomic_abstractions[i]->is_solvable() && !continue_if_unsolvable) {
            cout << "Proved unsolvability by: " << *(atomic_abstractions[i])
                 << endl;
            res_abstractions.push_back(atomic_abstractions[i]);
            return;
        }
    }

    // Combinations of variables that we have already explored (to avoid
    // repeating the analysis of a var subset)
    set<set<int>> explored_varsets;

    // Generate the instance of multiple linear merge strategy to select
    // variables
    MultipleLinearMergeStrategy order(
        merge_criteria,
        merge_order,
        true /*is_first*/);
    vector<int> selected_variables;
    get_candidate_vars(selected_variables, explored_varsets);
    cout << selected_variables.size() << " candidate vars: ";
    order.next_vars(selected_variables);
    cout << selected_variables.size() << " selected vars" << endl;
    for (size_t i = 0;
         i < selected_variables.size() && i <= (size_t)max_branching_merge;
         i++) {
        int var_no = selected_variables[i];
        Abstraction* abstraction = atomic_abstractions[var_no];
        prepare_after_merge(abstraction, order.reduce_labels_before_merge());
        if (build_abstraction_dfs(
                abstraction,
                order,
                explored_varsets,
                res_abstractions)) {
            return;
        }
    }
}

// Returns true if proved unsolvability
bool MergeAndShrinkHeuristic::build_abstraction_dfs(
    Abstraction* abstraction,
    MultipleLinearMergeStrategy& order,
    set<set<int>>& explored_varsets,
    vector<Abstraction*>& res_abstractions) const
{

    cout << "DFS: " << *abstraction << endl;
    // prepare_before_varsel(abstraction);
    vector<int> selected_variables;
    get_candidate_vars(selected_variables, explored_varsets, abstraction);
    cout << selected_variables.size() << " candidate vars: ";
    order.next_vars(selected_variables, abstraction);
    cout << selected_variables.size() << " selected vars" << endl;

    if (selected_variables.empty()) {
        // Reached a leaf
        if (c_release_memory) {
            abstraction->release_memory();
        }
        res_abstractions.push_back(abstraction);
    } else {
        for (size_t i = 0; i < selected_variables.size(); i++) {
            int var_no = selected_variables[i];
            Abstraction* other_abstraction = atomic_abstractions[var_no];
            prepare_before_merge(abstraction, other_abstraction);
            Abstraction* new_abstraction = new CompositeAbstraction(
                is_unit_cost(),
                get_cost_type(),
                abstraction,
                other_abstraction);

            if (prune_unreachable || prune_irrelevant ||
                shrink_strategy->require_distances()) {
                new_abstraction->compute_distances(
                    prune_unreachable,
                    prune_irrelevant,
                    continue_if_unsolvable,
                    use_uniform_distances);
                if (!new_abstraction->is_solvable() &&
                    !continue_if_unsolvable) {
                    if (c_release_memory) {
                        new_abstraction->release_memory();
                    }
                    res_abstractions.push_back(new_abstraction);
                    return true; // Proved unsolvability
                }
                new_abstraction->statistics(false);
            }

            prepare_after_merge(
                new_abstraction,
                order.reduce_labels_before_merge());

            if (prune_unreachable || prune_irrelevant ||
                shrink_strategy->require_distances()) {
                new_abstraction->compute_distances(
                    prune_unreachable,
                    prune_irrelevant,
                    continue_if_unsolvable,
                    use_uniform_distances);
                new_abstraction->statistics(false);
            }

            bool finished = false;
            if (!new_abstraction->is_solvable() && !continue_if_unsolvable) {
                cout << "Proved unsolvability by new_abs: " << *new_abstraction
                     << endl;
                res_abstractions.push_back(new_abstraction);
                finished = true;
            }
            if (finished || build_abstraction_dfs(
                                new_abstraction,
                                order,
                                explored_varsets,
                                res_abstractions)) {
                if (c_release_memory) {
                    new_abstraction->release_memory();
                }
                return true; // Proved unsolvability
            }
            new_abstraction->release_memory();
        }
    }
    return false;
}

void MergeAndShrinkHeuristic::build_atomic_abstractions()
{
    Abstraction::build_atomic_abstractions(
        is_unit_cost(),
        get_cost_type(),
        atomic_abstractions);

    cout << "Shrinking atomic abstractions..." << endl;
    for (size_t i = 0; i < atomic_abstractions.size(); ++i) {
        atomic_abstractions[i]->statistics(false);
        if (empty_label_shrink_strategy) {
            // Note: this works only for linear merging strategies, as the
            // labels of an atomic abstraction' transitions cannot be empty for
            // the variables in the MAIN abstraction
            cout << "applying empty label shrinking" << endl;
            empty_label_shrink_strategy->shrink_atomic(*atomic_abstractions[i]);
            if (atomic_abstractions[i]->size() == 1) {
                atomic_abstractions[i]->mark_irrelevant();
            }
        } else {
            if (check_unsolvability) {
                // only relevant for checking unsolvability
                const vector<int>& new_vars =
                    g_causal_graph->get_predecessors(i);
                if (new_vars.empty()) {
                    // variable i is root of CG if its DTG is additionally a
                    // single SCC, we can ignore this variable as it will not
                    // change anything in terms of solvability
                    if (atomic_abstractions[i]->is_scc()) {
                        atomic_abstractions[i]->mark_irrelevant();
                    }
                }
            }
        }

        // no need to shrink if abstraction will be ignored anyway
        if (!atomic_abstractions[i]->is_irrelevant()) {
            atomic_abstractions[i]->compute_distances(
                prune_unreachable,
                prune_irrelevant,
                continue_if_unsolvable,
                use_uniform_distances);
            if (!atomic_abstractions[i]->is_solvable() &&
                !continue_if_unsolvable) {
                cout << "Number of merging steps: 0" << endl;
                cout << "Last abstraction step: -1/" << g_variable_domain.size()
                     << " vars" << endl;
                return;
            }
            shrink_strategy->shrink_atomic(*atomic_abstractions[i]);
        }
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // atomic_abstractions[i]->statistics(false);
    }
    /*int num_irrelevant_variables = 0;
      for (int i = 0; i < irrelevant_abstractions.size(); i++) {
      if (atomic_abstractions[i]->is_irrelevant())
      num_irrelevant_variables++;
      }
      cout << "Number of irrelevant variables: " << num_irrelevant_variables <<
      endl; exit(1);*/
}

Abstraction* MergeAndShrinkHeuristic::build_abstraction(bool is_first)
{
    // TODO: We're leaking memory here in various ways. Fix this.
    //       Don't forget that build_atomic_abstractions also
    //       allocates memory.

    if (empty_label_shrink_strategy) {
        assert(check_unsolvability == true);
    }

    vector<Abstraction*> atom_abstractions;
    Abstraction::build_atomic_abstractions(
        is_unit_cost(),
        get_cost_type(),
        atom_abstractions);

    vector<bool> irrelevant_abstractions(atom_abstractions.size(), false);
    cout << "Shrinking atomic abstractions..." << endl;
    for (size_t i = 0; i < atom_abstractions.size(); ++i) {
        atom_abstractions[i]->statistics(false);
        if (empty_label_shrink_strategy) {
            // Note: this works only for linear merging strategies, as the
            // labels of an atomic abstraction' transitions cannot be empty for
            // the variables in the MAIN abstraction
            cout << "applying empty label shrinking" << endl;
            empty_label_shrink_strategy->shrink_atomic(*atom_abstractions[i]);
            if (atom_abstractions[i]->size() == 1) {
                irrelevant_abstractions[i] = true;
            }
        } else {
            if (check_unsolvability) {
                // only relevant for checking unsolvability
                const vector<int>& new_vars =
                    g_causal_graph->get_predecessors(i);
                if (new_vars.empty()) {
                    // variable i is root of CG
                    // if its DTG is additionally a single SCC, we can ignore
                    // this variable as it will not change anything in terms of
                    // solvability
                    irrelevant_abstractions[i] = atom_abstractions[i]->is_scc();
                }
            }
        }
        atom_abstractions[i]->compute_distances(
            prune_unreachable,
            prune_irrelevant,
            continue_if_unsolvable,
            use_uniform_distances);
        if (!atom_abstractions[i]->is_solvable() && !continue_if_unsolvable) {
            cout << "Number of merging steps: 0" << endl;
            cout << "Last abstraction step: -1/" << g_variable_domain.size()
                 << " vars" << endl;
            return atom_abstractions[i];
        }
        if (!irrelevant_abstractions[i]) { // no need to shrink if abstraction
                                           // will be ignored anyway
            shrink_strategy->shrink_atomic(*atom_abstractions[i]);
        }
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // atom_abstractions[i]->statistics(false);
    }

    /*int num_irrelevant_variables = 0;
    for (int i = 0; i < irrelevant_abstractions.size(); i++) {
        if (irrelevant_abstractions[i])
                num_irrelevant_variables++;
    }
    cout << "Number of irrelevant variables: " << num_irrelevant_variables <<
    endl; exit(1);*/

    cout << "Merging abstractions..." << endl;

    // TODO: We should use a unique pointer here
    VariableOrderFinderInterface* order;

    if (merge_strategy == MERGE_LINEAR) {
        order = new LinearMergeStrategy(merge_criteria, merge_order, is_first);
    } else {
        order = new VariableOrderFinder(merge_strategy, is_first);
    }

    int var_no = 0;
    while (!order->done()) {
        var_no = order->next();
        if (irrelevant_abstractions[var_no]) {
            cout << "Ignoring variable: #" << var_no << endl;
            atom_abstractions[var_no]->release_memory();
        } else {
            break;
        }
    }
    cout << "First variable: #" << var_no;
    for (size_t i = 0; i < g_fact_names[var_no].size(); ++i) {
        cout << " " << g_fact_names[var_no][i];
    }
    cout << endl;
    Abstraction* abstraction = atom_abstractions[var_no];
    // Peter: number of states and/or transitions cannot have increased, so no
    // need to update statistics here
    // abstraction->statistics(use_expensive_statistics);
    int merge_count = 0;

#ifdef DEBUG_ABSTRACTIONS
    int abs_count = 0;
#endif

    while (!order->done()) {
        cout << "DFS1: " << *abstraction << endl;
        // TODO: When using nonlinear merge strategies, make sure not
        // to normalize multiple parts of a composite. See issue68.
        if (shrink_strategy->reduce_labels_before_shrinking() ||
            order->reduce_labels_before_merge() || use_label_inheritance ||
            empty_label_shrink_strategy) {

            abstraction->normalize(
                use_label_reduction,
                shrink_strategy->get_caught_labels_set(),
                use_label_inheritance,
                new_approach);
            // Note: label inheritance for atomic abstractions works only for
            // linear merging strategies, as the label set cannot be empty for
            // the MAIN abstraction
            //     other_abstraction->normalize(false,
            //     shrink_strategy->get_caught_labels_set(),
            // 				 use_label_inheritance, new_approach);
        }

        if (empty_label_shrink_strategy) {
            cout << "applying empty label shrinking" << endl;
            // HACK, we need two abstractions for shrink_before_merge, so
            // instead we call shrink_atomic (it does the same in the empty
            // label shrinking)
            // empty_label_shrink_strategy->shrink_before_merge(*abstraction,
            // *other_abstraction);
            empty_label_shrink_strategy->shrink_atomic(*abstraction);
            // TODO: When using nonlinear merge strategies, make sure not
            // to normalize multiple parts of a composite. See issue68.
            if (shrink_strategy->reduce_labels_before_shrinking() ||
                use_label_inheritance) {
                abstraction->normalize(
                    false /*use_label_reduction*/ /*no need to reduce labels
                                                     again -- transitions with
                                                     same labels are already
                                                     merged!*/
                    ,
                    shrink_strategy->get_caught_labels_set(),
                    use_label_inheritance,
                    new_approach);
                // other_abstraction->normalize(false,
                // shrink_strategy->get_caught_labels_set(),
                // use_label_inheritance, new_approach);
            } else {
                /* just to keep the statistics up to date */
                // Peter: number of states and/or transitions cannot have
                // increased, so no need to update statistics here
                // abstraction->statistics(use_expensive_statistics);
                // other_abstraction->statistics(use_expensive_statistics);
            }
        }

        abstraction->compute_distances(
            prune_unreachable,
            prune_irrelevant,
            continue_if_unsolvable,
            use_uniform_distances);
        if (!abstraction->is_solvable() && !continue_if_unsolvable) {
            cout << "Number of merging steps: " << merge_count << endl;
            cout << "Last abstraction step: " << (merge_count + 1) << "/"
                 << g_variable_domain.size() << " vars" << endl;
            delete order;
            return abstraction;
        }
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // abstraction->statistics(use_expensive_statistics);

#ifdef DEBUG_ABSTRACTIONS
        // Dumping the abstract state space into dot file
        ostringstream out;
        out << "abstract.space"
            << "." << abs_count << ".dot";
        ofstream myfile;
        myfile.open(out.str().c_str(), ios::out);
        if (myfile.is_open()) {
            abstraction->dump(myfile);
            myfile.close();
        } else {
            cout << "Unable to open dot file!" << endl;
            exit(1);
        }
#endif
        // SELECT NEXT VARIABLE MOVED HERE
        int var2_no = order->next(abstraction);
        if (irrelevant_abstractions[var2_no]) {
            cout << "Ignoring next variable: #" << var2_no << endl;
            atom_abstractions[var2_no]->release_memory();
            continue;
        }
        cout << "Next variable: #" << var2_no;
        for (size_t i = 0; i < g_fact_names[var2_no].size(); ++i) {
            cout << " " << g_fact_names[var2_no][i];
        }
        cout << endl;
        Abstraction* other_abstraction = atom_abstractions[var2_no];

        if (shrink_strategy->reduce_labels_before_shrinking() ||
            use_label_inheritance || empty_label_shrink_strategy) {
            // Note: label inheritance for atomic abstractions works only for
            // linear merging strategies, as the label set cannot be empty for
            // the MAIN abstraction
            other_abstraction->normalize(
                false,
                shrink_strategy->get_caught_labels_set(),
                use_label_inheritance,
                new_approach);
        }
        if (empty_label_shrink_strategy) {
            cout << "applying empty label shrinking" << endl;
            empty_label_shrink_strategy->shrink_before_merge(
                *abstraction,
                *other_abstraction);
            // TODO: When using nonlinear merge strategies, make sure not
            // to normalize multiple parts of a composite. See issue68.
            if (shrink_strategy->reduce_labels_before_shrinking() ||
                use_label_inheritance) {
                other_abstraction->normalize(
                    false,
                    shrink_strategy->get_caught_labels_set(),
                    use_label_inheritance,
                    new_approach);
            }
        }

        other_abstraction->compute_distances(
            prune_unreachable,
            prune_irrelevant,
            continue_if_unsolvable,
            use_uniform_distances);
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // other_abstraction->statistics(use_expensive_statistics);
#ifdef DEBUG_ABSTRACTIONS
        cout << "DUMPING abstraction BEFORE SHRINKING" << endl;
        abstraction->dump();
        cout << "DUMPING other_abstraction BEFORE SHRINKING" << endl;
        other_abstraction->dump();
#endif
        shrink_strategy->shrink_before_merge(*abstraction, *other_abstraction);

#ifdef DEBUG_ABSTRACTIONS
        // Commented out since we shouldn't usually need/want this, even when
        // debugging. It will also give us warnings since the abstraction is not
        // normalized here.
        /*
        cout << "DUMPING abstraction AFTER SHRINKING" << endl;
        abstraction->dump();
        cout << "DUMPING other_abstraction AFTER SHRINKING" << endl;
        other_abstraction->dump();
        */

        ostringstream out2;
        out2 << "abstract.space.after.shrink"
             << "." << abs_count++ << ".dot";
        ofstream myfile2;
        myfile2.open(out2.str().c_str(), ios::out);
        if (myfile2.is_open()) {
            abstraction->dump(myfile2);
            myfile2.close();
        } else {
            cout << "Unable to open dot file!" << endl;
            exit(1);
        }
#endif

        // TODO: Make shrink_before_merge return a pair of bools
        //       that tells us whether they have actually changed,
        //       and use that to decide whether to dump statistics?
        // (The old code would print statistics on abstraction iff it was
        // shrunk. This is not so easy any more since this method is not
        // in control, and the shrink strategy doesn't know whether we want
        // expensive statistics. As a temporary aid, we just print the
        // statistics always now, whether or not we shrunk.)
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // abstraction->statistics(use_expensive_statistics);
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // other_abstraction->statistics(use_expensive_statistics);

        /* apply label inheritance only once, i.e., before the shrinking step */
        /* PETER: Why do we use label reduction again -- should the transitions
         * with equal labels not already be merged, at least if
         * shrink-strategy->reduce_labels_before_shrinking() returns true?! */
        abstraction->normalize(
            use_label_reduction,
            shrink_strategy->get_caught_labels_set(),
            false);
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // abstraction->statistics(use_expensive_statistics);

        // Don't label-reduce the atomic abstraction -- see issue68.
        other_abstraction->normalize(
            false,
            shrink_strategy->get_caught_labels_set(),
            false);
        // Peter: number of states and/or transitions cannot have increased, so
        // no need to update statistics here
        // other_abstraction->statistics(use_expensive_statistics);

        int peak_memory =
            max(abstraction->get_peak_memory_estimate(),
                other_abstraction->get_peak_memory_estimate());
        int max_abstract_states =
            max(abstraction->get_max_num_states(),
                other_abstraction->get_max_num_states());
        int max_abstract_transitions =
            max(abstraction->get_max_num_transitions(),
                other_abstraction->get_max_num_transitions());
        int overall_peak_memory =
            max(abstraction->get_overall_peak_memory_estimate(),
                other_abstraction->get_overall_peak_memory_estimate());
        int overall_max_abstract_states =
            max(abstraction->get_overall_max_num_states(),
                other_abstraction->get_overall_max_num_states());
        int overall_max_abstract_transitions =
            max(abstraction->get_overall_max_num_transitions(),
                other_abstraction->get_overall_max_num_transitions());
        cout << "Estimated peak memory for this abstraction: " << peak_memory
             << " bytes" << endl;
        cout << "Maximal number of states in this abstraction: "
             << max_abstract_states << endl;
        cout << "Maximal number of transitions in this abstraction: "
             << max_abstract_transitions << endl;

        // abstraction->dump();

        Abstraction* new_abstraction = new CompositeAbstraction(
            is_unit_cost(),
            get_cost_type(),
            abstraction,
            other_abstraction);

        abstraction->release_memory();
        other_abstraction->release_memory();

        abstraction = new_abstraction;
        // number of states and/or transitions must be updated
        abstraction->statistics(use_expensive_statistics);
        abstraction->set_overall_peak_memory_estimate(overall_peak_memory);
        abstraction->set_overall_max_num_states(overall_max_abstract_states);
        abstraction->set_overall_max_num_transitions(
            overall_max_abstract_transitions);

        // abstraction->dump();

        merge_count++;
        /* this can help pruning unreachable/irrelevant states before starting
         * on label reduction problem before: label reduction ran out of memory
         * if unreachable/irrelevant states not pruned beforehand (at least in
         * some instances) possible downside: Too many transitions here
         */
        if (prune_unreachable || prune_irrelevant ||
            shrink_strategy->require_distances()) {
            abstraction->compute_distances(
                prune_unreachable,
                prune_irrelevant,
                continue_if_unsolvable,
                use_uniform_distances);
            if (!abstraction->is_solvable() && !continue_if_unsolvable) {
                cout << "Number of merging steps: " << merge_count << endl;
                cout << "Last abstraction step: " << (merge_count + 1) << "/"
                     << g_variable_domain.size() << " vars" << endl;
                delete order;
                return abstraction;
            }
            abstraction->statistics(false);
        }
    }

    cout << "Number of merging steps: " << merge_count << endl;
    cout << "Last abstraction step: " << (merge_count + 1) << "/"
         << g_variable_domain.size() << " vars" << endl;

    delete order;

    /*bool force_recalculation = false;
    if (use_uniform_distances && !is_unit_cost_problem()) {
        if (shrink_strategy->name() != "no_shrink" ||
                        !shrink_strategy->is_goal_leading()) {
            // force recalculation of distances and use normal action costs if
    so far BFS was forced and original actions are not unit-cost
                force_recalculation = true;
        }
    }
    if (force_recalculation)
        abstraction->compute_distances(prune_unreachable, prune_irrelevant,
    continue_if_unsolvable, false, true); else*/
    abstraction->compute_distances(
        prune_unreachable,
        prune_irrelevant,
        continue_if_unsolvable,
        use_uniform_distances);
    if (!abstraction->is_solvable() && !continue_if_unsolvable) {
        cout << "Number of merging steps: " << merge_count << endl;
        cout << "Last abstraction step: " << (merge_count + 1) << "/"
             << g_variable_domain.size() << " vars" << endl;
        return abstraction;
    }

    std::shared_ptr<ShrinkStrategy> def_shrink =
        ShrinkFH::create_default(abstraction->size());
    def_shrink->shrink(*abstraction, abstraction->size(), true);
    /*if (force_recalculation)
        abstraction->compute_distances(prune_unreachable, prune_irrelevant,
    continue_if_unsolvable); else*/
    abstraction->compute_distances(
        prune_unreachable,
        prune_irrelevant,
        continue_if_unsolvable,
        use_uniform_distances);

    abstraction->statistics(use_expensive_statistics);
    if (c_release_memory && (shrink_strategy->name() != "no_shrink" ||
                             !shrink_strategy->is_goal_leading())) {
        abstraction->release_memory();
    }
    return abstraction;
}

void MergeAndShrinkHeuristic::initialize()
{
    utils::Timer timer;
    cout << "Initializing merge-and-shrink heuristic..." << endl;
    dump_options();
    warn_on_unusual_options();

    legacy::verify_no_axioms_no_conditional_effects();

    if (g_variable_domain.empty() || g_operators.empty()) {
        cout << "Problem detected as unsolvable by preprocessor" << endl;
        return;
    }
    int peak_memory = 0;
    int max_abstract_states = 0;
    int max_abstract_transitions = 0;

    if (merge_strategy == MERGE_LINEAR_MULTIPLE_DFS) {
        build_multiple_abstractions(abstractions);
    } else {

        for (int i = 0; i < abstraction_count; i++) {
            cout << "Building abstraction #" << (i + 1) << "..." << endl;
            Abstraction* abstraction = build_abstraction(i == 0);
            peak_memory =
                max(peak_memory,
                    abstraction->get_overall_peak_memory_estimate());
            max_abstract_states =
                max(max_abstract_states,
                    abstraction->get_overall_max_num_states());
            max_abstract_transitions =
                max(max_abstract_transitions,
                    abstraction->get_overall_max_num_transitions());
            abstractions.push_back(abstraction);
            if (!abstractions.back()->is_solvable()) {
                cout << "Abstract problem is unsolvable!" << endl;
                if (i + 1 < abstraction_count) {
                    cout << "Skipping remaining abstractions." << endl;
                }
                break;
            }
        }
    }

    cout << "Done initializing merge-and-shrink heuristic [" << timer << "]"
         << endl
         << "initial h value: " << compute_heuristic(g_initial_state()) << endl;
    cout << "Estimated peak memory for abstraction: " << peak_memory << " bytes"
         << endl;
    cout << "Maximal number of states in abstractions: " << max_abstract_states
         << endl;
    cout << "Maximal number of transitions in abstractions: "
         << max_abstract_transitions << endl;
    cout << "Total abstraction time: " << timer << endl;
    cout << "Generated " << abstractions.size() << " abstractions" << endl;
    // exit(0);
    // Michael: for goal leading actions
    if (shrink_strategy->name() == "no_shrink" &&
        shrink_strategy->is_goal_leading()) {
        cout << "##############################################################"
                "##########################"
             << endl;
        for (size_t i = 0; i < abstractions.size(); i++) {
            // abstractions[i]->dump_goal_leading_actions(shrink_strategy->is_backward_pruned());
            abstractions[i]->write_action_files(
                shrink_strategy->is_backward_pruned());
#ifdef DEBUG_ABSTRACTIONS
            ofstream myfile("state.space.dot");
            if (myfile.is_open()) {
                abstractions[i]->dump(myfile);
                myfile.close();
            } else {
                cout << "Unable to open actions file!" << endl;
                exit(1);
            }
#endif
            // Released memory only here, since it could be needed for goal
            // leading actions
            for (size_t j = 0; j < abstractions.size(); j++) {
                abstractions[j]->release_memory();
            }
        }
    } else {
#ifdef DEBUG_ABSTRACTIONS
        for (size_t i = 0; i < abstractions.size(); i++) {
            ofstream myfile("abstract.space.final.dot");
            if (myfile.is_open()) {
                abstractions[i]->dump(myfile);
                myfile.close();
            } else {
                cout << "Unable to open actions file!" << endl;
                exit(1);
            }
        }
#endif
    }
}

int MergeAndShrinkHeuristic::compute_heuristic(const GlobalState& state)
{
    if (check_unsolvability) {
        // in case we are only interested in detecting dead-ends, we can ignore
        // the actual costs unless they are infinity
        for (size_t i = 0; i < abstractions.size(); i++) {
            int abs_cost = abstractions[i]->get_cost(state);
            if (abs_cost == -1) {
                return DEAD_END;
            }
        }
        return 0;
    }
    int cost = 0;
    for (size_t i = 0; i < abstractions.size(); i++) {
        int abs_cost = abstractions[i]->get_cost(state);
        if (abs_cost == -1) {
            return DEAD_END;
        }
        cost = max(cost, abs_cost);
    }
    return cost;
}

static std::shared_ptr<Heuristic> _parse(options::OptionParser& parser)
{
    // TODO: better documentation what each parameter does
    parser.add_option<int>("count", "nr of abstractions to build", "1");
    vector<string> merge_strategies;
    // TODO: it's a bit dangerous that the merge strategies here
    //  have to be specified exactly in the same order
    //  as in the enum definition. Try to find a way around this,
    //  or at least raise an error when the order is wrong.
    merge_strategies.push_back("MERGE_LINEAR_CG_GOAL_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_CG_GOAL_RANDOM");
    merge_strategies.push_back("MERGE_LINEAR_GOAL_CG_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_RANDOM");
    merge_strategies.push_back("MERGE_DFP");
    merge_strategies.push_back("MERGE_LINEAR_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR_REVERSE_LEVEL");
    merge_strategies.push_back("MERGE_LINEAR");
    merge_strategies.push_back("MERGE_LINEAR_MULTIPLE_DFS");

    parser.add_enum_option<MergeStrategy>(
        "merge_strategy",
        merge_strategies,
        "merge strategy",
        "MERGE_LINEAR");

    vector<string> merge_orders;
    merge_orders.push_back("level");
    merge_orders.push_back("reverse_level");
    merge_orders.push_back("random");
    parser.add_enum_option<MergeOrder>(
        "merge_order",
        merge_orders,
        "merge variable order for tie breaking",
        "RANDOM");

    // TODO: Default shrink strategy should only be created
    // when it's actually used.
    parser.add_option<std::shared_ptr<ShrinkStrategy>>(
        "shrink_strategy",
        "shrink strategy",
        "default");

    parser.add_list_option<std::shared_ptr<MergeCriterion>>(
        "merge_criteria",
        "merge strategy criteria",
        "[]");
    // TODO: Rename option name to "use_label_reduction" to be
    //       consistent with the papers?
    parser.add_option<bool>("reduce_labels", "enable label reduction", "true");
    parser.add_option<bool>(
        "prune_unreachable",
        "prune unreachable states (WARNING: setting this to false it not fully "
        "supported)",
        "true");
    parser.add_option<bool>(
        "prune_irrelevant",
        "prune irrelevant states (WARNING: setting this to false it not fully "
        "supported)",
        "true");
    parser.add_option<bool>(
        "expensive_statistics",
        "show statistics on \"unique unlabeled edges\" (WARNING: "
        "these are *very* slow -- check the warning in the output)",
        "false");
    parser.add_option<bool>(
        "label_inheritance",
        "apply label inheritance",
        "false");
    /*PETER: REMOVE THIS!*/
    parser.add_option<bool>("new_approach", "", "false");
    parser.add_option<bool>("use_empty_label_shrinking", "", "false");
    parser.add_option<bool>("use_uniform_distances", "", "false");
    parser.add_option<bool>("check_unsolvability", "", "false");
    parser.add_option<bool>("continue_if_unsolvable", "", "false");

    parser.add_option<int>(
        "merge_size_limit",
        "Maximum size of abstractions after a merge in "
        "MERGE_LINEAR_MULTIPLE_DFS",
        "10000");

    parser.add_option<int>(
        "max_branching_merge",
        "Maximum branching factor in MERGE_LINEAR_MULTIPLE_DFS",
        std::to_string(numeric_limits<int>::max()));

    Heuristic::add_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (parser.help_mode()) {
        return 0;
    }

    MergeStrategy merge_strategy = opts.get<MergeStrategy>("merge_strategy");
    if (merge_strategy < 0 || merge_strategy >= MAX_MERGE_STRATEGY) {
        cerr << "error: unknown merge strategy: " << merge_strategy << endl;
        exit(2);
    }

    if (parser.dry_run()) {
        return 0;
    } else {
        return std::make_shared<MergeAndShrinkHeuristic>(opts);
    }
}

static std::shared_ptr<Heuristic> _parse_ownk(options::OptionParser& parser)
{
    Heuristic::add_options_to_parser(parser);
    parser.add_option<int>("k", "", "100000");
    options::Options opts = parser.parse();
    if (!parser.dry_run()) {
        opts.set<int>("cost_type", 3);

        std::vector<std::shared_ptr<MergeCriterion>> merge_criteria;
        merge_criteria.push_back(
            std::make_shared<MergeCriterionMinSCC>(false, false));
        merge_criteria.push_back(std::make_shared<MergeCriterionGoal>());
        opts.set<std::vector<std::shared_ptr<MergeCriterion>>>(
            "merge_criteria",
            merge_criteria);

        opts.set<int>("merge_order", 0);

        options::Options tmp1;
        tmp1.set<int>("cost_type", 1);
        tmp1.set<int>("max_states", std::numeric_limits<int>::max());
        tmp1.set<int>("max_states_before_merge", opts.get<int>("k"));
        tmp1.set<int>("max_states_before_catching", opts.get<int>("k"));
        tmp1.set<int>("greedy", 0);
        tmp1.set<int>("threshold", 1);
        tmp1.set<bool>("initialize_by_h", true);
        tmp1.set<bool>("group_by_h", false);
        tmp1.set<int>("at_limit", 0);
        tmp1.set<int>("goal_leading", 7);
        tmp1.set<double>("alpha", 1.0);
        tmp1.set<bool>("use_all_best_supporters", true);
        ShrinkBisimulation::handle_option_defaults(tmp1);
        opts.set<std::shared_ptr<ShrinkStrategy>>(
            "shrink_strategy",
            std::make_shared<ShrinkLabelSubsetBisimulation>(tmp1));

        opts.set<bool>("reduce_labels", true);
        opts.set<int>("count", 1);
        opts.set<int>("merge_strategy", 7);
        opts.set<bool>("use_empty_label_shrinking", false);
        opts.set<bool>("use_uniform_distances", true);
        opts.set<bool>("check_unsolvability", true);
        opts.set<bool>("prune_unreachable", true);
        opts.set<bool>("prune_irrelevant", true);
        opts.set<bool>("expensive_statistics", false);
        opts.set<bool>("label_inheritance", false);
        opts.set<bool>("new_approach", false);
        opts.set<bool>("continue_if_unsolvable", false);
        opts.set<int>("merge_size_limit", 10000);
        opts.set<int>("max_branching_merge", std::numeric_limits<int>::max());

        return std::make_shared<MergeAndShrinkHeuristic>(opts);
    }

    return 0;
}

static std::shared_ptr<Heuristic> _parse_owna(options::OptionParser& parser)
{
    Heuristic::add_options_to_parser(parser);
    options::Options opts = parser.parse();
    if (!parser.dry_run()) {
        opts.set<int>("cost_type", 3);

        std::vector<std::shared_ptr<MergeCriterion>> merge_criteria;
        merge_criteria.push_back(
            std::make_shared<MergeCriterionMinSCC>(false, false));
        merge_criteria.push_back(std::make_shared<MergeCriterionGoal>());
        opts.set<std::vector<std::shared_ptr<MergeCriterion>>>(
            "merge_criteria",
            merge_criteria);

        opts.set<int>("merge_order", 0);

        options::Options tmp1;
        tmp1.set<int>("cost_type", 1);
        tmp1.set<int>("max_states", std::numeric_limits<int>::max());
        tmp1.set<int>(
            "max_states_before_merge",
            std::numeric_limits<int>::max());
        tmp1.set<int>("greedy", 0);
        tmp1.set<int>("threshold", 1);
        tmp1.set<bool>("initialize_by_h", true);
        tmp1.set<bool>("group_by_h", false);
        tmp1.set<int>("at_limit", 0);
        ShrinkBisimulation::handle_option_defaults(tmp1);
        opts.set<std::shared_ptr<ShrinkStrategy>>(
            "shrink_strategy",
            std::make_shared<ShrinkBisimulation>(tmp1));

        opts.set<bool>("reduce_labels", true);
        opts.set<int>("count", 1);
        opts.set<int>("merge_strategy", 7);
        opts.set<bool>("use_empty_label_shrinking", true);
        opts.set<bool>("use_uniform_distances", true);
        opts.set<bool>("check_unsolvability", true);
        opts.set<bool>("prune_unreachable", true);
        opts.set<bool>("prune_irrelevant", true);
        opts.set<bool>("expensive_statistics", false);
        opts.set<bool>("label_inheritance", false);
        opts.set<bool>("new_approach", false);
        opts.set<bool>("continue_if_unsolvable", false);
        opts.set<int>("merge_size_limit", 10000);
        opts.set<int>("max_branching_merge", std::numeric_limits<int>::max());

        return std::make_shared<MergeAndShrinkHeuristic>(opts);
    }

    return 0;
}

static Plugin<Heuristic> _plugin("legacy_merge_and_shrink", _parse);
static Plugin<Heuristic> _plugin_onwk("legacy_msownk", _parse_ownk);
static Plugin<Heuristic> _plugin_onwa("legacy_msowna", _parse_owna);

} // namespace merge_and_shrink
} // namespace legacy