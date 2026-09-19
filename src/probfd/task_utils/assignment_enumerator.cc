#include "probfd/task_utils/assignment_enumerator.h"

namespace probfd::enumeration {

unsigned AssignmentEnumerator::num_assignments() const
{
    return num_assignments_;
}

unsigned AssignmentEnumerator::num_vars() const
{
    return var_infos_.size();
}

int AssignmentEnumerator::rank_fact(int idx, int val) const
{
    return var_infos_[idx].multiplier * val;
}

std::vector<int> AssignmentEnumerator::unrank(int assignment_index) const
{
    std::vector<int> values(var_infos_.size());
    for (size_t i = 0; i != var_infos_.size(); ++i) {
        values[i] = value_of(assignment_index, i);
    }
    return values;
}

int AssignmentEnumerator::value_of(int assignment_index, int idx) const
{
    const auto& [multiplier, domain] = var_infos_[idx];
    return (assignment_index / multiplier) % domain;
}

bool AssignmentEnumerator::next_index(
    int& assignment_index,
    std::span<int> mutable_variables) const
{
    for (const int var : mutable_variables) {
        const auto& [multiplier, domain] = var_infos_[var];
        const int value = (assignment_index / multiplier) % domain;

        if (value + 1 < domain) {
            assignment_index += multiplier;
            return true;
        }

        assignment_index -= value * multiplier;
    }

    return false;
}

long long int AssignmentEnumerator::get_multiplier(int var) const
{
    return var_infos_[var].multiplier;
}

int AssignmentEnumerator::get_domain_size(int var) const
{
    return var_infos_[var].domain;
}

} // namespace probfd::pdbs
