#include "probfd/merge_and_shrink/factored_mapping.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;

namespace probfd::merge_and_shrink {

FactoredMapping::FactoredMapping(int domain_size)
    : lookup_table(domain_size)
{
    std::iota(lookup_table.begin(), lookup_table.end(), 0);
}

int FactoredMapping::get_num_abstract_states() const
{
    return std::ranges::max(lookup_table) + 1;
}

void FactoredMapping::apply_abstraction(const vector<int>& abstraction_mapping)
{
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) {
            entry = abstraction_mapping[entry];
        }
    }
}

void FactoredMapping::scale(int scale)
{
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) {
            entry *= scale;
        }
    }
}

FactoredMappingAtomic::FactoredMappingAtomic(int var_id, int domain_size)
    : FactoredMapping(domain_size)
    , var_id(var_id)
{
}

int FactoredMappingAtomic::get_abstract_state(const State& state) const
{
    return lookup_table[state[var_id].get_value()];
}

bool FactoredMappingAtomic::is_total() const
{
    return !utils::contains(lookup_table, PRUNED_STATE);
}

void FactoredMappingAtomic::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract state lookup table (leaf): ";
        for (const int value : lookup_table) {
            log << value << ", ";
        }
        log << endl;
    }
}

FactoredMappingMerge::FactoredMappingMerge(
    unique_ptr<FactoredMapping> left_child_,
    unique_ptr<FactoredMapping> right_child_)
    : FactoredMapping(
          left_child_->get_num_abstract_states() *
          right_child_->get_num_abstract_states())
    , left_child(std::move(left_child_))
    , right_child(std::move(right_child_))
{
    right_child->scale(left_child->get_num_abstract_states());
}

int FactoredMappingMerge::get_abstract_state(const State& state) const
{
    int state1 = left_child->get_abstract_state(state);
    int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return PRUNED_STATE;
    return lookup_table[state1 + state2];
}

bool FactoredMappingMerge::is_total() const
{
    return !utils::contains(lookup_table, PRUNED_STATE) &&
           left_child->is_total() && right_child->is_total();
}

void FactoredMappingMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract state lookup table (merge): " << endl;

        auto it = lookup_table.begin();
        auto end = lookup_table.end();

        while (it != end) {
            auto sub_end = it + left_child->get_num_abstract_states();
            assert(it != sub_end);
            log << *it;
            for (++it; it != sub_end; ++it) {
                log << ", " << *it;
            }
            log << endl;
        }

        log << "left child:" << endl;
        left_child->dump(log);
        log << "right child:" << endl;
        right_child->dump(log);
    }
}

std::pair<int, int> FactoredMappingMerge::get_children_states(int state) const
{
    return {
        state % left_child->get_num_abstract_states(),
        state / left_child->get_num_abstract_states()};
}

} // namespace probfd::merge_and_shrink