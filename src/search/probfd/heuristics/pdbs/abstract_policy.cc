#include "probfd/heuristics/pdbs/abstract_policy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

AbstractPolicy::AbstractPolicy(std::size_t num_abstract_states)
    : optimal_operators(num_abstract_states)
{
}

AbstractPolicy::OperatorList& AbstractPolicy::operator[](StateRank state)
{
    return optimal_operators[state.id];
}

const AbstractPolicy::OperatorList&
AbstractPolicy::operator[](StateRank state) const
{
    return optimal_operators[state.id];
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd