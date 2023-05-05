#ifndef PROBFD_HEURISTICS_PDBS_ABSTRACT_POLICY_H
#define PROBFD_HEURISTICS_PDBS_ABSTRACT_POLICY_H

#include "probfd/heuristics/pdbs/abstract_operator.h"
#include "probfd/heuristics/pdbs/state_rank.h"


#include <vector>

namespace probfd {
namespace heuristics {

namespace pdbs {

struct AbstractOperator;

class AbstractPolicy {
public:
    using OperatorList = std::vector<const AbstractOperator*>;

private:
    std::vector<OperatorList> optimal_operators;

public:
    AbstractPolicy(std::size_t num_abstract_states);

    OperatorList& operator[](StateRank state);
    const OperatorList& operator[](StateRank state) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif