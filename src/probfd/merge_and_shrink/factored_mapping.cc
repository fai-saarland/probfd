#include "probfd/merge_and_shrink/factored_mapping.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/types.h"

#include "downward/utils/logging.h"

#include <algorithm>
#include <cassert>
#include <numeric>

using namespace std;
using namespace downward;

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
    return lookup_table[state[var_id]];
}

bool FactoredMappingAtomic::is_total() const
{
    return !std::ranges::contains(lookup_table, PRUNED_STATE);
}

void FactoredMappingAtomic::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log.println("abstract state lookup table (leaf): {}", lookup_table);
    }
}

FactoredMappingMerge::FactoredMappingMerge(
    unique_ptr<FactoredMapping> left_child,
    unique_ptr<FactoredMapping> right_child)
    : FactoredMapping(
          left_child->get_num_abstract_states() *
          right_child->get_num_abstract_states())
    , left_child(std::move(left_child))
    , right_child(std::move(right_child))
{
    this->right_child->scale(this->left_child->get_num_abstract_states());
}

int FactoredMappingMerge::get_abstract_state(const State& state) const
{
    const int state1 = left_child->get_abstract_state(state);
    const int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return PRUNED_STATE;
    return lookup_table[state1 + state2];
}

bool FactoredMappingMerge::is_total() const
{
    return !std::ranges::contains(lookup_table, PRUNED_STATE) &&
           left_child->is_total() && right_child->is_total();
}

void FactoredMappingMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log.println("abstract state lookup table (merge): ");

        auto it = lookup_table.begin();
        const auto end = lookup_table.end();

        while (it != end) {
            auto sub_end = it + left_child->get_num_abstract_states();
            assert(it != sub_end);
            log.println("{}", std::ranges::subrange(it, sub_end));
            it = sub_end;
        }

        log.println("left child:");
        left_child->dump(log);
        log.println("right child:");
        right_child->dump(log);
    }
}

std::pair<int, int> FactoredMappingMerge::get_children_states(int state) const
{
    const auto it = std::ranges::find(lookup_table, state);
    assert(it != lookup_table.end());
    const auto index = std::distance(lookup_table.begin(), it);

    const auto left = index % left_child->get_num_abstract_states();
    const auto right = index / left_child->get_num_abstract_states();
    return {left, right};
}

} // namespace probfd::merge_and_shrink