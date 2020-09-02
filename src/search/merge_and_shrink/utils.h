#ifndef MERGE_AND_SHRINK_UTILS_H
#define MERGE_AND_SHRINK_UTILS_H

#include <vector>
#include <set>

namespace merge_and_shrink {

extern void handle_marked_operators(
		std::vector<std::set<int> >& touched_states,
		int& op_no,
		std::vector<std::set<int> >& touching_operators,
		int& num_states_done,
		std::vector<bool>& labels,
		int& num_actions);

extern void mark_relevant_operators(
		int& num_states,
		std::vector<std::set<int> >& touching_operators,
		std::vector<std::set<int> >& touched_states,
		std::vector<bool>& labels,
		int& num_actions);

}

#endif
