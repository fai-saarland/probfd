#include "probfd/heuristics/pdbs/projection_policy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

ProjectionPolicy::ProjectionPolicy(std::size_t num_abstract_states)
    : optimal_operators(num_abstract_states)
{
}

ProjectionPolicy::OperatorList& ProjectionPolicy::operator[](StateRank state)
{
    return optimal_operators[state.id];
}

const ProjectionPolicy::OperatorList&
ProjectionPolicy::operator[](StateRank state) const
{
    return optimal_operators[state.id];
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd