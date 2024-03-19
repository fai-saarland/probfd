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

int FactoredMapping::get_domain_size() const
{
    return std::ranges::max(lookup_table) + 1;
}

int FactoredMapping::get_table_size() const
{
    return static_cast<int>(lookup_table.size());
}

void FactoredMapping::scale(int scale)
{
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) {
            entry *= scale;
        }
    }
}

void FactoredMapping::apply_abstraction(const vector<int>& abstraction_mapping)
{
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) {
            entry = abstraction_mapping[entry];
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

std::unique_ptr<MergeAndShrinkDistanceRepresentation>
FactoredMappingAtomic::create_distance_representation(
    const Distances& distances)
{
    auto r = std::make_unique<MergeAndShrinkDistanceRepresentationLeaf>(
        var_id,
        lookup_table,
        distances);
    lookup_table.clear();
    return r;
}

FactoredMappingMerge::FactoredMappingMerge(
    unique_ptr<FactoredMapping> left_child_,
    unique_ptr<FactoredMapping> right_child_)
    : FactoredMapping(
          left_child_->get_domain_size() * right_child_->get_domain_size())
    , left_child(std::move(left_child_))
    , right_child(std::move(right_child_))
{
    right_child->scale(left_child->get_domain_size());
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
            auto sub_end = it + left_child->get_table_size();
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

std::unique_ptr<MergeAndShrinkDistanceRepresentation>
FactoredMappingMerge::create_distance_representation(const Distances& distances)
{
    auto r = std::make_unique<MergeAndShrinkDistanceRepresentationMerge>(
        std::move(left_child),
        std::move(right_child),
        lookup_table,
        distances);
    lookup_table.clear();
    return r;
}

MergeAndShrinkDistanceRepresentation::MergeAndShrinkDistanceRepresentation(
    std::size_t table_size)
    : lookup_table(table_size)
{
}

MergeAndShrinkDistanceRepresentationLeaf::
    MergeAndShrinkDistanceRepresentationLeaf(
        int var_id,
        std::vector<int>& state_lookup_table,
        const Distances& distances)
    : MergeAndShrinkDistanceRepresentation(state_lookup_table.size())
    , var_id(var_id)
{
    assert(distances.are_goal_distances_computed());
    for (size_t i = 0; i != state_lookup_table.size(); ++i) {
        const int abstract_state = state_lookup_table[i];
        lookup_table[i] = abstract_state != PRUNED_STATE
                              ? distances.get_goal_distance(abstract_state)
                              : INFINITE_VALUE;
    }
}

value_t MergeAndShrinkDistanceRepresentationLeaf::get_abstract_distance(
    const State& state) const
{
    return lookup_table[state[var_id].get_value()];
}

void MergeAndShrinkDistanceRepresentationLeaf::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract distance lookup table (leaf): ";
        for (const value_t value : lookup_table) {
            log << value << ", ";
        }
        log << endl;
    }
}

MergeAndShrinkDistanceRepresentationMerge::
    MergeAndShrinkDistanceRepresentationMerge(
        std::unique_ptr<FactoredMapping> left_child_,
        std::unique_ptr<FactoredMapping> right_child_,
        std::vector<int>& state_lookup,
        const Distances& distances)
    : MergeAndShrinkDistanceRepresentation(state_lookup.size())
    , left_child(std::move(left_child_))
    , right_child(std::move(right_child_))
{
    using namespace std::views;
    assert(distances.are_goal_distances_computed());
    for (auto [abstract_state, lookup] : zip(state_lookup, lookup_table)) {
        lookup = abstract_state != PRUNED_STATE
                     ? distances.get_goal_distance(abstract_state)
                     : INFINITE_VALUE;
    }
}

value_t MergeAndShrinkDistanceRepresentationMerge::get_abstract_distance(
    const State& state) const
{
    int state1 = left_child->get_abstract_state(state);
    int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return INFINITE_VALUE;
    return lookup_table[state1 + state2];
}

void MergeAndShrinkDistanceRepresentationMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract distance lookup table (merge): " << endl;
        auto it = lookup_table.begin();
        auto end = lookup_table.end();

        while (it != end) {
            auto sub_end = it + left_child->get_table_size();
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

} // namespace probfd::merge_and_shrink