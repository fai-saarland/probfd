#ifndef LEGACY_MERGE_AND_SHRINK_UTILS_H
#define LEGACY_MERGE_AND_SHRINK_UTILS_H

#include <set>
#include <vector>

namespace legacy {
namespace merge_and_shrink {

extern void handle_marked_operators(
    std::vector<std::set<int>>& touched_states,
    int& op_no,
    std::vector<std::set<int>>& touching_operators,
    int& num_states_done,
    std::vector<bool>& labels,
    int& num_actions);

extern void mark_relevant_operators(
    int& num_states,
    std::vector<std::set<int>>& touching_operators,
    std::vector<std::set<int>>& touched_states,
    std::vector<bool>& labels,
    int& num_actions);

} // namespace merge_and_shrink
} // namespace legacy

#endif
