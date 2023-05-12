#ifndef PROBFD_HEURISTICS_PDBS_ABSTRACT_POLICY_H
#define PROBFD_HEURISTICS_PDBS_ABSTRACT_POLICY_H

#include "probfd/heuristics/pdbs/projection_operator.h"
#include "probfd/heuristics/pdbs/state_rank.h"


#include <vector>

namespace probfd {
namespace heuristics {

namespace pdbs {

class ProjectionOperator;

class ProjectionPolicy {
public:
    using OperatorList = std::vector<const ProjectionOperator*>;

private:
    std::vector<OperatorList> optimal_operators;

public:
    ProjectionPolicy(std::size_t num_abstract_states);

    OperatorList& operator[](StateRank state);
    const OperatorList& operator[](StateRank state) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif