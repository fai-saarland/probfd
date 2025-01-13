#include "downward/transformations/domain_abstraction.h"

#include "downward/abstract_task.h"
#include "downward/utils/collections.h"

namespace downward {

DomainAbstraction::DomainAbstraction(std::vector<std::vector<int>> value_map)
    : value_map(std::move(value_map))
{
}

void DomainAbstraction::convert_ancestor_state_values(
    std::vector<int>& values) const
{
    int num_vars = static_cast<int>(values.size());
    for (int var = 0; var < num_vars; ++var) {
        int old_value = values[var];
        int new_value = value_map[var][old_value];
        values[var] = new_value;
    }
}

int DomainAbstraction::get_abstract_value(const FactPair& fact) const
{
    assert(utils::in_bounds(fact.var, value_map));
    assert(utils::in_bounds(fact.value, value_map[fact.var]));
    return value_map[fact.var][fact.value];
}

FactPair DomainAbstraction::get_abstract_fact(const FactPair& fact) const
{
    return FactPair(fact.var, get_abstract_value(fact));
}

}