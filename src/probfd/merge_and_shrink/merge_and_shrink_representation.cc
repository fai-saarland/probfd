#include "probfd/merge_and_shrink/merge_and_shrink_representation.h"

#include "probfd/merge_and_shrink/types.h"
#include "probfd/merge_and_shrink/distances.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <cassert>
#include <numeric>

using namespace std;

namespace probfd::merge_and_shrink {

MergeAndShrinkRepresentation::MergeAndShrinkRepresentation(int domain_size)
    : domain_size(domain_size)
{
}

int MergeAndShrinkRepresentation::get_domain_size() const
{
    return domain_size;
}

MergeAndShrinkRepresentationLeaf::MergeAndShrinkRepresentationLeaf(
    int var_id,
    int domain_size)
    : MergeAndShrinkRepresentation(domain_size)
    , var_id(var_id)
    , lookup_table(domain_size)
{
    iota(lookup_table.begin(), lookup_table.end(), 0);
}

void MergeAndShrinkRepresentationLeaf::apply_abstraction_to_lookup_table(
    const vector<int>& abstraction_mapping)
{
    int new_domain_size = 0;
    for (int& entry : lookup_table) {
        if (entry != PRUNED_STATE) {
            entry = abstraction_mapping[entry];
            new_domain_size = max(new_domain_size, entry + 1);
        }
    }
    domain_size = new_domain_size;
}

int MergeAndShrinkRepresentationLeaf::get_abstract_state(
    const State& state) const
{
    return lookup_table[state[var_id].get_value()];
}

bool MergeAndShrinkRepresentationLeaf::is_total() const
{
    for (const int entry : lookup_table) {
        if (entry == PRUNED_STATE) {
            return false;
        }
    }

    return true;
}

void MergeAndShrinkRepresentationLeaf::dump(utils::LogProxy& log) const
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
MergeAndShrinkRepresentationLeaf::create_distance_representation(
    const Distances& distances)
{
    auto r = std::make_unique<MergeAndShrinkDistanceRepresentationLeaf>(
        var_id,
        domain_size,
        lookup_table,
        distances);
    lookup_table.clear();
    return r;
}

MergeAndShrinkRepresentationMerge::MergeAndShrinkRepresentationMerge(
    unique_ptr<MergeAndShrinkRepresentation> left_child,
    unique_ptr<MergeAndShrinkRepresentation> right_child)
    : MergeAndShrinkRepresentation(
          left_child->get_domain_size() * right_child->get_domain_size())
    , left_child(std::move(left_child))
    , right_child(std::move(right_child))
    , lookup_table(
          this->left_child->get_domain_size(),
          vector<int>(this->right_child->get_domain_size()))
{
    int counter = 0;
    for (vector<int>& row : lookup_table) {
        for (int& abstract_state : row) {
            abstract_state = counter++;
        }
    }
}

void MergeAndShrinkRepresentationMerge::apply_abstraction_to_lookup_table(
    const vector<int>& abstraction_mapping)
{
    int new_domain_size = 0;
    for (vector<int>& row : lookup_table) {
        for (int& entry : row) {
            if (entry != PRUNED_STATE) {
                entry = abstraction_mapping[entry];
                new_domain_size = max(new_domain_size, entry + 1);
            }
        }
    }
    domain_size = new_domain_size;
}

int MergeAndShrinkRepresentationMerge::get_abstract_state(
    const State& state) const
{
    const int state1 = left_child->get_abstract_state(state);
    const int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return PRUNED_STATE;
    return lookup_table[state1][state2];
}

bool MergeAndShrinkRepresentationMerge::is_total() const
{
    for (const vector<int>& row : lookup_table) {
        for (const int entry : row) {
            if (entry == PRUNED_STATE) {
                return false;
            }
        }
    }

    return left_child->is_total() && right_child->is_total();
}

void MergeAndShrinkRepresentationMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract state lookup table (merge): " << endl;
        for (const auto& row : lookup_table) {
            for (const auto& value : row) {
                log << value << ", ";
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
MergeAndShrinkRepresentationMerge::create_distance_representation(
    const Distances& distances)
{
    auto r = std::make_unique<MergeAndShrinkDistanceRepresentationMerge>(
        domain_size,
        std::move(left_child),
        std::move(right_child),
        lookup_table,
        distances);
    lookup_table.clear();
    return r;
}

MergeAndShrinkDistanceRepresentation::MergeAndShrinkDistanceRepresentation(
    int domain_size)
    : domain_size(domain_size)
{
}

MergeAndShrinkDistanceRepresentationLeaf::
    MergeAndShrinkDistanceRepresentationLeaf(
        int var_id,
        int domain_size,
        const std::vector<int>& state_lookup_table,
        const Distances& distances)
    : MergeAndShrinkDistanceRepresentation(domain_size)
    , var_id(var_id)
    , lookup_table(domain_size)
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
        int domain_size,
        std::unique_ptr<MergeAndShrinkRepresentation> left_child,
        std::unique_ptr<MergeAndShrinkRepresentation> right_child,
        std::vector<std::vector<int>>& state_lookup_table,
        const Distances& distances)
    : MergeAndShrinkDistanceRepresentation(domain_size)
    , left_child(std::move(left_child))
    , right_child(std::move(right_child))
    , lookup_table(
          this->left_child->get_domain_size(),
          vector<value_t>(this->right_child->get_domain_size()))
{
    assert(distances.are_goal_distances_computed());
    for (size_t i = 0; i != state_lookup_table.size(); ++i) {
        vector<int>& row = state_lookup_table[i];
        vector<value_t>& distance_row = lookup_table[i];
        for (size_t j = 0; j != row.size(); ++j) {
            const int abstract_state = row[j];

            distance_row[j] = abstract_state != PRUNED_STATE
                                  ? distances.get_goal_distance(abstract_state)
                                  : INFINITE_VALUE;
        }
    }
}

value_t MergeAndShrinkDistanceRepresentationMerge::get_abstract_distance(
    const State& state) const
{
    const int state1 = left_child->get_abstract_state(state);
    const int state2 = right_child->get_abstract_state(state);
    if (state1 == PRUNED_STATE || state2 == PRUNED_STATE) return INFINITE_VALUE;
    return lookup_table[state1][state2];
}

void MergeAndShrinkDistanceRepresentationMerge::dump(utils::LogProxy& log) const
{
    if (log.is_at_least_debug()) {
        log << "abstract distance lookup table (merge): " << endl;
        for (const auto& row : lookup_table) {
            for (const auto& value : row) {
                log << value << ", ";
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