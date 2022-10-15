#include "merge_and_shrink/shrink_label_subset_bisimulation.h"

#include "merge_and_shrink/abstraction.h"
#include "merge_and_shrink/max_regression_operators.h"
#include "merge_and_shrink/relaxed_plan_operators.h"

#include "utils/timer.h"

#include "globals.h"
#include "option_parser.h"
#include "plugin.h"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

namespace merge_and_shrink {

ShrinkLabelSubsetBisimulation::ShrinkLabelSubsetBisimulation(
    const options::Options &opts)
    : ShrinkBisimulation(opts),
      goal_leading(GoalLeading(opts.get_enum("goal_leading"))),
      max_states_before_catching(opts.get_enum("max_states_before_catching"))
{

    cout << goal_leading << endl;

    if (goal_leading == ALL_ACTIONS) {
        set_all_labels();
        goal_leading_threshold_reached = true;
    } else if (goal_leading == GOAL_LEADING_FROM_THRESHOLD ||
               goal_leading == GOAL_LEADING_FROM_THRESHOLD_BACKWARD_PRUNED ||
               goal_leading == SHORTEST_PATH_RELEVANT_FROM_THRESHOLD ||
               goal_leading == SHORTEST_PATH_RELEVANT_FROM_THRESHOLD_BACKWARD_PRUNED ||
               goal_leading == PATH_RELEVANT_FROM_THRESHOLD) {
        set_all_labels();
        goal_leading_threshold_reached = false;
    } else if (goal_leading == ALL_GOAL_LEADING ||
               goal_leading == BACKWARD_PRUNED_GOAL_LEADING) {
        set_labels_from_file();
        goal_leading_threshold_reached = true;
    } else if (goal_leading == APPROXIMATED_BY_H_MAX ||
               goal_leading == APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED) {

        /*    	// If no backward pruning is performed - no bound is needed
            	if (goal_leading == APPROXIMATED_BY_H_MAX)
            		opts.set("alpha", -1.0);
        //    		opts.set("alpha", numeric_limits<double>::max());
        */
        // Creating the "heuristic" here, getting the labels
        utils::Timer timer;
        MaxRegressionOperators *reg_op = new MaxRegressionOperators(opts);
//    	reg_op->evaluate(*g_initial_state);
        reg_op->get_relaxed_operators(reg_labels);
        cout << "Finding label set took: " << timer << endl;

        goal_leading_threshold_reached = true;
    } else if (goal_leading == APPROXIMATED_BY_H_FF) {
        utils::Timer timer;
        RelaxedPlanOperators *rel_plan_op = new RelaxedPlanOperators(opts);
        rel_plan_op->get_relaxed_plan_operators(reg_labels);
        cout << "Finding label set took: " << timer << endl;

        goal_leading_threshold_reached = true;
    } else {
        abort();
    }

}

ShrinkLabelSubsetBisimulation::~ShrinkLabelSubsetBisimulation()
{
}

string ShrinkLabelSubsetBisimulation::name() const
{
    return "label subset bisimulation";
}

void ShrinkLabelSubsetBisimulation::dump_strategy_specific_options() const
{


//	cout << "Bisimulation type: ";
    cout << endl << "Bisimulation is" ;
    if (goal_leading == ALL_ACTIONS) {
        cout << " not";
    } else if (goal_leading == ALL_GOAL_LEADING) {
        cout << "";
    } else if (goal_leading == BACKWARD_PRUNED_GOAL_LEADING) {
        cout << " backward pruned";
    } else if (goal_leading == GOAL_LEADING_FROM_THRESHOLD) {
        cout << " from threshold";
    } else if (goal_leading == GOAL_LEADING_FROM_THRESHOLD_BACKWARD_PRUNED) {
        cout << " backward pruned from threshold";
    } else if (goal_leading == SHORTEST_PATH_RELEVANT_FROM_THRESHOLD) {
        cout << " shortest-path relevant from threshold";
    } else if (goal_leading ==
               SHORTEST_PATH_RELEVANT_FROM_THRESHOLD_BACKWARD_PRUNED) {
        cout << " backward pruned shortest-path relevant from threshold";
    } else if (goal_leading == PATH_RELEVANT_FROM_THRESHOLD) {
        cout << " path relevant from threshold";
    } else if (goal_leading == APPROXIMATED_BY_H_MAX) {
        cout << " approximation by h_max - no bound";
    } else if (goal_leading == APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED) {
        cout << " backward pruned approximation by h_max";
    } else if (goal_leading == APPROXIMATED_BY_H_FF) {
        cout << " approximation by h_ff";
    } else {
        abort();
    }

    cout << " goal leading" << endl;


    if (goal_leading == ALL_ACTIONS ||
            goal_leading == ALL_GOAL_LEADING ||
            goal_leading == BACKWARD_PRUNED_GOAL_LEADING ||
            goal_leading == APPROXIMATED_BY_H_MAX ||
            goal_leading == APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED ||
            goal_leading == APPROXIMATED_BY_H_FF) {
        // In those cases the set of labels to catch here is already known
        dump_caught_labels_statistic();
    }

//    cout << "Read goal leading actions "<< to_catch << " out of total number of actions " << g_operators.size() << endl;
    ShrinkBisimulation::dump_strategy_specific_options();

}

bool ShrinkLabelSubsetBisimulation::reduce_labels_before_shrinking() const
{
    return goal_leading_threshold_reached;
}


void ShrinkLabelSubsetBisimulation::dump_caught_labels_statistic() const
{

    int to_catch = 0;
    const std::vector<bool> &caught = get_caught_labels_set();

    for (size_t i = 0; i < caught.size(); i++) {
        to_catch += caught[i];
#ifdef DEBUG_ABSTRACTIONS
        // dumping caught labels
        if (caught[i]) {
            cout << "Caught!:  " ;
        }
        g_operators[i].dump();
#endif
    }

    cout << "Using goal leading actions " << to_catch <<
         " out of total number of actions " << g_operators.size() << endl;

}


void ShrinkLabelSubsetBisimulation::shrink_before_merge(
    Abstraction &abs1, Abstraction &abs2)
{
    pair<int, int> new_sizes = compute_shrink_sizes(abs1.size(), abs2.size());
    int new_size1 = new_sizes.first;
    int new_size2 = new_sizes.second;

    // Michael : Here comes the check if the limit is not reached yet.
    // If it is reached, the goal leading actions for the abstraction that reached
    // the limit are found and stored.
    // From this point on every shrink uses this set of actions as an approximation
    // for the actual goal leading actions
    //if (!goal_leading_threshold_reached && (new_size1 != abs1.size() || abs1.size() > max_states_before_catching)) {
    if (!goal_leading_threshold_reached
            && abs1.size() > max_states_before_catching) {
        // Emptying previously full of "true" values vector of labels to use
//    	cout << "======================================================================" << endl;
//    	cout << "Threshold is reached for the first time - goal leading actions are set" << endl;
        if (goal_leading == GOAL_LEADING_FROM_THRESHOLD ||
                goal_leading == GOAL_LEADING_FROM_THRESHOLD_BACKWARD_PRUNED) {
            set_labels_from_abstraction(abs1, is_backward_pruned(), 0);
        } else if (goal_leading == SHORTEST_PATH_RELEVANT_FROM_THRESHOLD ||
                   goal_leading == SHORTEST_PATH_RELEVANT_FROM_THRESHOLD_BACKWARD_PRUNED) {
            set_labels_from_abstraction(abs1, is_backward_pruned(), 1);
        } else if (goal_leading == PATH_RELEVANT_FROM_THRESHOLD) {
            set_labels_from_abstraction(abs1, is_backward_pruned(), 2);
        }
        // Dumping statistics for getting the caught labels info
        dump_caught_labels_statistic();
        goal_leading_threshold_reached = true;
    }

    // HACK: The output is based on the assumptions of a linear merge
    //       strategy. It would be better (and quite possible) to
    //       treat both abstractions exactly the same here by amending
    //       the output a bit.
    if (new_size2 != abs2.size()) {
        cout << "atomic abstraction too big; must shrink" << endl;
    }
    shrink(abs2, new_size2);
    shrink(abs1, new_size1);
}


const std::vector<bool> &ShrinkLabelSubsetBisimulation::get_caught_labels_set()
const
{
    if (goal_leading == APPROXIMATED_BY_H_MAX ||
            goal_leading == APPROXIMATED_BY_H_MAX_BACKWARD_PRUNED ||
            goal_leading == APPROXIMATED_BY_H_FF) {
        return reg_labels;
    }
    return ShrinkBisimulation::get_caught_labels_set();
}

void ShrinkLabelSubsetBisimulation::handle_option_defaults(options::Options &opts)
{
    ShrinkBisimulation::handle_option_defaults(opts);

    /* Peter: Wondered what this is about. Basically, in case of approximation by
     * h_max, we want to consider all facts, even those with a value greater than
     * that of the initial state. Thus, we have to use the highest possible bound.
     */
    GoalLeading gl = GoalLeading(opts.get_enum("goal_leading"));
    if (gl == APPROXIMATED_BY_H_MAX) {
        opts.set<double>("alpha", numeric_limits<double>::max());
    }
}


static std::shared_ptr<ShrinkStrategy> _parse(options::OptionParser &parser)
{
    Heuristic::add_options_to_parser(parser);
    ShrinkBisimulation::add_options_to_parser(parser);
    vector<string> goal_lead;
    goal_lead.push_back("false");
    goal_lead.push_back("all");
    goal_lead.push_back("backward_pruning");
    goal_lead.push_back("from_threshold");
    goal_lead.push_back("from_threshold_backward_pruning");
    goal_lead.push_back("shortest_path_relevant_from_threshold");
    goal_lead.push_back("shortest_path_relevant_from_threshold_backward_pruning");
    goal_lead.push_back("path_relevant_from_threshold");
    goal_lead.push_back("from_h_max");
    goal_lead.push_back("from_h_max_backward_pruning");
    goal_lead.push_back("from_h_ff");
    parser.add_enum_option(
        "goal_leading", goal_lead,
        "a method for setting the set of labels to catch", "false");

    parser.add_option<double>("alpha", "alpha", "1.0");

    parser.add_option<bool>("use_all_best_supporters",
                            "for approximation by h_max: "
                            "should we use all best supporters, or only one for each fact?", "true");

    parser.add_option<int>("max_states_before_catching",
                           "the limit at which to calculate the threshold-based label sets",
                           std::to_string(numeric_limits<int>::max()));

    options::Options opts = parser.parse();

    if (!parser.dry_run()) {
        ShrinkLabelSubsetBisimulation::handle_option_defaults(opts);
        return std::make_shared<ShrinkLabelSubsetBisimulation>(opts);
    } else {
        return 0;
    }
}

static Plugin<ShrinkStrategy> _plugin("shrink_label_subset_bisimulation",
                                      _parse);

}
