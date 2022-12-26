#include "probfd/heuristics/pdbs/abstract_policy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

AbstractPolicy::AbstractPolicy(std::size_t num_abstract_states)
    : optimal_operators(num_abstract_states, OperatorList())
{
}

AbstractPolicy::OperatorList&
AbstractPolicy::operator[](const StateRank& state)
{
    return optimal_operators[state.id];
}

const AbstractPolicy::OperatorList&
AbstractPolicy::operator[](const StateRank& state) const
{
    return optimal_operators[state.id];
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd