#include "merge_and_shrink/max_regression_operators.h"

#include "merge_and_shrink/utils.h"

#include "global_operator.h"
#include "global_state.h"
#include "option_parser.h"

#include <cassert>
#include <vector>
using namespace std;

namespace merge_and_shrink {


// construction and destruction
MaxRegressionOperators::MaxRegressionOperators(const options::Options &opts)
    : HSPMaxHeuristic(opts), alpha(opts.get<double>("alpha")),
      all_best_supporters(opts.get<bool>("use_all_best_supporters"))
{

}

MaxRegressionOperators::~MaxRegressionOperators()
{
}

/*
void MaxRegressionOperators::get_relaxed_operators(vector<bool>& labels) {
	// Go over the propositions in the backward queue, for each action that
	// achieves a proposition, add its preconditions to the queue,
	// giving it the cost of operator + the cost of the proposition in the effect.
	// We stop when there is no change to the backward queue (that is, nothing
	// more to add).
	// We return the actions that have unary effect representatives such that all their
	// preconditions have cost under the bound and the effect is strictly under the bound.
	// The more strict approach of getting only the actions with all their
	// representatives having such a condition does not seem to work,
	// because of the -1 in the precondition (see drop action in Gripper for example)

	evaluate(*g_initial_state);

//	int bound = numeric_limits<int>::max();
//	if (alpha < numeric_limits<double>::max())
//		bound = (int) get_value() * alpha;

	//Changed to double.
	double bound = numeric_limits<double>::max();
	if (alpha < numeric_limits<double>::max())
		bound = get_value() * alpha;

    // Cross-reference unary operators
    for (int i = 0; i < unary_operators.size(); i++) {
        UnaryOperator *op = &unary_operators[i];
        op->effect->effect_of.push_back(op);
        op->cost = op->base_cost; // will be increased by precondition costs
    }

    for (int var = 0; var < propositions.size(); var++) {
        for (int value = 0; value < propositions[var].size(); value++) {
            Proposition &prop = propositions[var][value];
            prop.cost = -1;
//            // Dumping propositions
//            cout << "["<<var << "=" << value << "] ==> ";
//            prop.dump();
        }
    }

	reached_props.clear();

	// Starting with goals, putting them into reached propositions
    for (int i = 0; i < goal_propositions.size(); i++) {
    	enqueue_if_necessary(goal_propositions[i], 0);
    }

	//TODO: Implement the case of alpha = 0 separately. In this case we need to collect only minimal cost operators that achieve some goal.
    if (alpha == 0.0) {

    }

    // Setting propositions costs by going backwards from the goal propositions
    int round = 0;
    // Getting the smallest action cost from the first round, to update the bound, if too small.
    int min_action_cost_round_one = numeric_limits<int>::max();
    int prop_count = 0;
    while (prop_count < reached_props.size()) {
//    	cout << "============ Round " << round << endl;
    	prop_count = reached_props.size();
    	// Copying the currently reached propositions to go over
    	set<Proposition*> curr_reached = reached_props;

        // Going over the backward reached propositions and finding new ones
    	for (set<Proposition*>::iterator it = curr_reached.begin(); it != curr_reached.end(); ++it) {
    		Proposition *prop = *it;
//    		prop->dump();
            const vector<UnaryOperator *> &triggering_operators = prop->effect_of;
            // For each operator that can achieve this proposition, add its
            // preconditions with the adapted cost.
//            cout << "Triggering ops:" << endl;
            for (int i = 0; i < triggering_operators.size(); i++) {
                UnaryOperator *unary_op = triggering_operators[i];
//                cout<< "ID: " << unary_op->operator_no <<
//                " Base cost:" << unary_op->base_cost <<
//                " Curr cost:" << unary_op->cost;

                unary_op->cost = max(unary_op->cost,
                                     unary_op->base_cost + prop->cost);

//                cout<< " Fixed cost: " << unary_op->cost << endl;
                std::vector<Proposition *> &prec = unary_op->precondition;
                for (int p = 0; p < prec.size(); p++) {
                    if (enqueue_if_necessary(prec[p], unary_op)) {
                    	// For the first round only - get the minimal action cost
                    	if (round == 0)
                    		min_action_cost_round_one = min(min_action_cost_round_one, unary_op->cost);
                    }
                }
            }
//	    	cout << "--------------------------------------------------" << endl;
    	}
    	round++;
    }

    // Updating the bound
    bound = max(bound, min_action_cost_round_one);
	cout << "Using bound " << bound << " for approximating goal leading labels" << endl;

//    cout << "==================== Operators==============" << endl;
    labels.clear();
    labels.assign(g_operators.size(),false);

    // Going over the unary operators, marking those that have all preconditions cost under the bound
    // and effect cost strictly under the bound
    for (int i = 0; i < unary_operators.size(); i++) {
        UnaryOperator *op = &unary_operators[i];
		if (op->operator_no < 0) // Skipping axioms
			continue;
		assert(op->operator_no < g_operators.size());

		// Changed to get operators from the first layer also in cse when bound is 0.
		if (op->effect->cost == -1 ||
				((op->effect->cost > 0) and (op->effect->cost >= bound)))
			continue;

//		if (op->effect->cost == -1 || op->effect->cost >= bound)
//			continue;

    	bool are_preconditions_bouded = true;
        std::vector<Proposition *> &prec = op->precondition;
        for (int p = 0; p < prec.size(); p++) {
        	if (prec[p]->cost == -1 || prec[p]->cost > bound) {
        		are_preconditions_bouded = false;
        		break;
        	}
        }
        if (!are_preconditions_bouded)
        	continue;

		labels[op->operator_no] = true;
//		// Dumping the actions - for testing
//		dump_unary_operator(op);
    }

}
*/


void MaxRegressionOperators::get_relaxed_operators(vector<bool> &labels)
{
    // Go over the propositions in the backward queue, for each action that
    // achieves a proposition, add its preconditions to the queue,
    // giving it the cost of operator + the cost of the proposition in the effect.
    // We stop when there is no change to the backward queue (that is, nothing
    // more to add).
    // We return the actions that have unary effect representatives such that all their
    // preconditions have cost under the bound and the effect is strictly under the bound.
    // The more strict approach of getting only the actions with all their
    // representatives having such a condition does not seem to work,
    // because of the -1 in the precondition (see drop action in Gripper for example)

    evaluate(g_initial_state());

    // Reseting propositions and operators
    reset();

//    cout << "==================== Operators==============" << endl;
    labels.clear();
    labels.assign(g_operators.size(), false);

    cout << "given alpha is " << alpha << "; using "
         << (all_best_supporters ? "all best supporters" : "one best supporter")
         << " for each fact" << endl;

    //The case of alpha = 0 is implemented separately. In this case we need to collect only minimal cost operators that achieve some goal.
    if (alpha == 0.0) {
        mark_minimal_set_of_relaxed_operators(labels);
        return;
    }

    double bound = numeric_limits<double>::max();
    if (alpha < numeric_limits<double>::max()) {
        bound = get_value() * alpha;
    }

    cout << "Heuristic value: " << get_value() << ", alpha: " << alpha <<
         ", calculated bound: " << bound << endl;

    // Setting propositions costs by going backwards from the goal propositions
    int min_action_cost_round_one = set_propostions_costs();

    // Updating the bound
    if (min_action_cost_round_one > bound) {
        bound = (double) min_action_cost_round_one;
    }
    cout << "Using bound " << bound << " for approximating goal leading labels" <<
         endl;

    mark_relaxed_operators(labels, bound);

}



void MaxRegressionOperators::mark_relaxed_operators(vector<bool> &labels,
        double bound)
{
    int num_props = 0;
    vector<set<int> > outgoing_operators;
    vector<set<int> > reached_propositions;
    if (!all_best_supporters) {
        for (size_t i = 0; i < propositions.size(); i++) {
            num_props += propositions[i].size();
        }
        outgoing_operators.resize(num_props);
        reached_propositions.resize(g_operators.size());
    }

    // We return the actions that have unary effect representatives such that all their
    // preconditions have cost under the bound and the effect is strictly under the bound.
    // In case the bound is 0 the second inequality is not strict.

    /*// Peter: If we want not necessarily all supporters for each fact, we can mark for which
    // facts we already marked a supporting action.
    for (int i = 0; i < propositions.size(); i++) {
    	for (int j = 0; j < propositions[i].size(); j++) {
    		propositions[i][j].marked_for_label_set_approximation = false;
    	}
    }*/

    // Going over the unary operators, marking those that have all preconditions cost under the bound
    // and effect cost strictly under the bound
    for (size_t i = 0; i < unary_operators.size(); i++) {
        UnaryOperator *op = &unary_operators[i];
        if (op->operator_no < 0) { // Skipping axioms
            continue;
        }
        assert((size_t) op->operator_no < g_operators.size());
        /*if (!all_best_supporters) {
        	// Peter: If we want not necessarily all supporters for each fact, we check whether
        	// the effect of this unary operator is already achieved based on the marked operators.
        	// If it is, we ignore it; otherwise we might mark it based on the same conditions
        	// as in the original code.
        	if (op->effect->marked_for_label_set_approximation) {
        		continue;
        	}
        }*/

        // Changed to get operators from the first layer also in case when bound is 0.
        if (op->effect->cost == -1 ||
                ((bound > 0) && (op->effect->cost >= bound))) {
            continue;
        }

        bool are_preconditions_bounded = true;
        std::vector<Proposition *> &prec = op->precondition;
        for (size_t p = 0; p < prec.size(); p++) {
            if (prec[p]->cost == -1 || prec[p]->cost > bound) {
                are_preconditions_bounded = false;
                break;
            }
        }
        if (!are_preconditions_bounded) {
            continue;
        }

        if (all_best_supporters) {
            labels[op->operator_no] = true;
        } else {
            /*labels[op->operator_no] = true;
            // Peter: mark all effect propositions of the original operator as already having a marked supporter
            // Peter: PROBLEM: This sets SOME supporter for each fact within the bound, not necessarily one of the BEST supporters.
            // However, if we use the transformation of all action costs to 0 for deciding unsolvability, this should be OK,
            // as in that case all achieving operators are best supporters.
            // Furthermore, M&S does not allow for conditional effects, so that all preconditions of the unary operators created
            // from one original operator must be the same.
            const vector<GlobalEffect> &prepost = g_operators[op->operator_no].get_pre_post();
            for (int j = 0; j < prepost.size(); j++) {
            	propositions[prepost[j].var][prepost[j].post].marked_for_label_set_approximation = true;
            }*/
            vector<Proposition *> &prec = op->precondition;
            for (size_t p = 0; p < prec.size(); p++) {
                if (prec[p]->cost == op->cost) {
                    // only consider best supporters
                    outgoing_operators[prec[p]->id].insert(op->operator_no);
                    reached_propositions[op->operator_no].insert(prec[p]->id);
                }
            }
        }
//		// Dumping the actions - for testing
//		dump_unary_operator(op);
    }
    int num_marked_operators = 0;
    mark_relevant_operators(num_props, outgoing_operators, reached_propositions,
                            labels, num_marked_operators);

}



void MaxRegressionOperators::mark_minimal_set_of_relaxed_operators(
    vector<bool> &labels)
{
    // We return the actions that have unary effect representatives such that their effect
    // is in goal, and their cost is the minimal among such.

    int min_cost = numeric_limits<int>::max();
    std::vector<int> cheapest;
    for (size_t i = 0; i < unary_operators.size(); i++) {
        UnaryOperator *op = &unary_operators[i];
        if (op->operator_no < 0) { // Skipping axioms
            continue;
        }
        assert((size_t) op->operator_no < g_operators.size());

        // Skipping non goal achieving operators
        if (!op->effect->is_goal) {
            continue;
        }

        if (op->cost <= min_cost) {
            if (op->cost < min_cost) {
                // Reseting the vector
                cheapest.clear();
                min_cost = op->cost;
            }
            // Adding this op index to the vector of cheapest ops
            cheapest.push_back(i);
        }
    }

    if (all_best_supporters) {
        // Peter: original implementation: take all minimum-cost actions achieving a goal fact
        for (size_t i = 0; i < cheapest.size(); i++) {
            UnaryOperator *op = &unary_operators[i];

            labels[op->operator_no] = true;
            //		// Dumping the actions - for testing
            //		dump_unary_operator(op);
        }
    } else {
        // Peter: new implementation: take one achieving minimum-cost action for each goal fact
        /*
         * Original idea: Go over all goal propositions. If some achieving minimum-cost action is
         * already caught, continue with next proposition. Otherwise mark first achieving action
         */
        /*for (int i = 0; i < goal_propositions.size(); i++) {
        	// check if at least one supporting action of this proposition is already marked
        	// and store the index (and cost) of the first cheapest one
        	const vector<UnaryOperator*> &triggering_operators = goal_propositions[i]->effect_of;
        	bool some_supporter_set = false;
        	int min_index = -1;
        	for (int j = 0; j < triggering_operators.size(); j++) {
        		UnaryOperator *op = triggering_operators[j];
        		if (op->cost == min_cost) {
        			// only these operators are of interest
        			if (labels[op->operator_no]) {
        				some_supporter_set = true;
        				break;
        			}
        			if (min_index == -1)
        				min_index = j;
        		}
        	}
        	assert(min_index != -1);
        	if (!some_supporter_set) {
        		// if not, set supporter to be the first of the cheapest operators
        		labels[goal_propositions[i]->effect_of[min_index]->operator_no] = true;
        	}
        }*/

        /*
         * New idea: For all facts predecessors of goal facts, collect outgoing operators that have a
         * goal fact in their effect.
         * Then first catch all operators for which there is one goal fact predecessor that is reached by regression
         * by only that operator. Finally, catch the operator reaching (by regression) the highest number of goal facts
         * until all goal fact predecessors are reached by regression by all caught operators.
         */
        /*
         * For all goal facts, store the achieving operators with minimum cost and their precondition facts
         * and the inverse, i.e., for such a minimum-cost operator the goal fact predecessors
         * reached by it by regression
         */
        int num_goal_props = goal_propositions.size();
        int num_props = 0;
        for (size_t i = 0; i < propositions.size(); i++) {
            num_props += propositions[i].size();
        }
        vector<set<int> > achieving_operators(num_props);
        vector<set<int> > achieved_propositions(g_operators.size());
        for (int i = 0; i < num_goal_props; i++) {
            const vector<UnaryOperator *> &triggering_operators =
                goal_propositions[i]->effect_of;
            for (size_t j = 0; j < triggering_operators.size(); j++) {
                UnaryOperator *op = triggering_operators[j];
                if (op->cost == min_cost) {
                    // only these operators are of interest
                    vector<Proposition *> &pre = op->precondition;
                    for (size_t k = 0; k < pre.size(); k++) {
                        achieving_operators[pre[k]->id].insert(op->operator_no);
                        achieved_propositions[op->operator_no].insert(pre[k]->id);
                    }
                }
            }
        }

        int num_marked_operators = 0;
        mark_relevant_operators(num_props, achieving_operators, achieved_propositions,
                                labels, num_marked_operators);
    }

}


void MaxRegressionOperators::reset()
{
    //Reseting the propositions and unary operators

    for (size_t var = 0; var < propositions.size(); var++) {
        for (size_t value = 0; value < propositions[var].size(); value++) {
            Proposition &prop = propositions[var][value];
            prop.effect_of.clear();
            prop.cost = -1;
//            // Dumping propositions
//            cout << "["<<var << "=" << value << "] ==> ";
//            prop.dump();
        }
    }

    // Cross-reference unary operators
    for (size_t i = 0; i < unary_operators.size(); i++) {
        UnaryOperator *op = &unary_operators[i];
        op->effect->effect_of.push_back(op);
        op->cost = op->base_cost; // will be increased by precondition costs
    }

    reached_props.clear();

    // Starting with goals, putting them into reached propositions
    for (size_t i = 0; i < goal_propositions.size(); i++) {
        enqueue_if_necessary(goal_propositions[i], 0);
    }
}


int MaxRegressionOperators::set_propostions_costs()
{
    // Setting propositions costs by going backwards from the goal propositions
    int round = 0;
    // Getting the smallest action cost from the first round, to update the bound, if too small.
    int min_action_cost_round_one = numeric_limits<int>::max();
    int prop_count = 0;
    while (prop_count < (int) reached_props.size()) {
//    	cout << "============ Round " << round << endl;
        prop_count = reached_props.size();
        // Copying the currently reached propositions to go over
        set<Proposition *> curr_reached = reached_props;

        // Going over the backward reached propositions and finding new ones
        for (set<Proposition *>::iterator it = curr_reached.begin();
                it != curr_reached.end(); ++it) {
            Proposition *prop = *it;
            assert(prop->cost >= 0);
//    		prop->dump();
            const vector<UnaryOperator *> &triggering_operators = prop->effect_of;
            // For each operator that can achieve this proposition, add its
            // preconditions with the adapted cost.
//            cout << "Triggering ops:" << endl;
            for (size_t i = 0; i < triggering_operators.size(); i++) {
                UnaryOperator *unary_op = triggering_operators[i];
//            	cout<< "ID: " << unary_op->operator_no <<
//            			" Base cost:" << unary_op->base_cost <<
//            			" Curr cost:" << unary_op->cost <<
//            			" Eff cost:" << prop->cost << endl;

//                if (unary_op->cost > unary_op->base_cost + prop->cost) {
//                	cout << "!!!!! Cost is too high!" << endl;
//                }

                unary_op->cost = max(unary_op->cost,
                                     unary_op->base_cost + prop->cost);

                std::vector<Proposition *> &prec = unary_op->precondition;
                for (size_t p = 0; p < prec.size(); p++) {
                    if (enqueue_if_necessary(prec[p], unary_op)) {
                        // For the first round only - get the minimal action cost
                        if (round == 0) {
                            min_action_cost_round_one = min(min_action_cost_round_one, unary_op->cost);
                        }
                    }
                }
            }
//	    	cout << "--------------------------------------------------" << endl;
        }
        round++;
    }
    return min_action_cost_round_one;
}

}
