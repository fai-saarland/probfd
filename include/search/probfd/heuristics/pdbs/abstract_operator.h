#ifndef PROBFD_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H
#define PROBFD_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H

#include "probfd/heuristics/pdbs/state_rank.h"

#include "probfd/distribution.h"
#include "probfd/task_proxy.h"

#include <string>
#include <vector>

namespace probfd {

class ProbabilisticOperator;

namespace heuristics {

/// Namespace dedicated to probabilistic pattern databases.
namespace pdbs {

struct AbstractOperator {
    explicit AbstractOperator(OperatorID id);
    OperatorID operator_id;
    Distribution<StateRank> outcomes;
};

class AbstractOperatorToString {
public:
    explicit AbstractOperatorToString(ProbabilisticTaskProxy task_proxy);
    std::string operator()(const AbstractOperator* op) const;

private:
    ProbabilisticTaskProxy task_proxy;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ABSTRACT_OPERATOR_H__