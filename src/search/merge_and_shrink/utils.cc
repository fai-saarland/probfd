#include "merge_and_shrink/utils.h"

using namespace std;

namespace merge_and_shrink {

void handle_marked_operators(
		vector<set<int> >& touched_states,
		int& op_no,
		vector<set<int> >& touching_operators,
		int& num_states_done,
		vector<bool>& labels,
		int& num_actions) {
	// find all states having this as relevant operator
	set<int> &relevantStates = touched_states[op_no];
	set<int>::iterator relevantStatesIt;
	for (relevantStatesIt = relevantStates.begin(); relevantStatesIt != relevantStates.end(); relevantStatesIt++) {
		// all these states are already done, so that they can be removed from any relevant operator
		int relevantState = *relevantStatesIt;
		set<int> &relevantOperators = touching_operators[relevantState];
		set<int>::iterator relevantOperatorsIt;
		for (relevantOperatorsIt = relevantOperators.begin(); relevantOperatorsIt != relevantOperators.end(); relevantOperatorsIt++) {
			int relevantOperator = *relevantOperatorsIt;
			touched_states[relevantOperator].erase(relevantState);
		}
		// as the state is done, we can remove the set of relevant operators from it
		touching_operators[relevantState].clear();
		num_states_done++;
	}
	labels[op_no] = true;
	num_actions++;
}

void mark_relevant_operators(
		int& num_states,
		vector<set<int> >& touching_operators,
		vector<set<int> >& touched_states,
		vector<bool>& labels,
		int& num_actions) {

	/*
	 * Store, how many states are already done, i.e., for which relevant states at least one relevant operator
	 * has been marked. All states without relevant operators are trivially done.
	 */
	int num_states_done = 0;
	for (int i = 0; i < num_states; i++) {
		if (touching_operators[i].empty())
			num_states_done++;
	}

	/*
	 * Check all states. If one has only one relevant operator, mark that and update the status of
	 * all relevant states of that action.
	 */
	for (int i = 0; i < num_states; i++) {
		if (touching_operators[i].size() == 1) {
			int op_no = *(touching_operators[i].begin());
			handle_marked_operators(touched_states, op_no, touching_operators, num_states_done, labels, num_actions);
		}
	}

	/*
	 * Take the first action with highest number of states with only unmarked relevant operators.
	 * Mark that and update the status of all relevant states of that action.
	 */
	while (num_states_done < num_states) {
		size_t max_num = 0;
		int max_index = -1;
		for (int i = 0; i < static_cast<int>(touched_states.size()); i++) {
			if (!touched_states[i].empty()) {
				size_t num_starting_states = touched_states[i].size();
				if (num_starting_states > max_num) {
					max_num = num_starting_states;
					max_index = i;
				}
			}
		}
		handle_marked_operators(touched_states, max_index, touching_operators, num_states_done, labels, num_actions);
	}
}

}

