#ifndef PROBFD_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H
#define PROBFD_HEURISTICS_PDBS_ABSTRACT_OPERATOR_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/distribution.h"
#include "probfd/task_proxy.h"

#include <string>
#include <vector>

namespace probfd {
namespace heuristics {

/// Namespace dedicated to probabilistic pattern databases.
namespace pdbs {

class ProjectionStateSpace;

/**
 * @brief Represents an operator of a projection state space.
 */
class ProjectionOperator {
    friend class ProjectionStateSpace;

    // Implementation detail. Like in the classical implementation, each
    // operator is applicable in exactly one state. The successor states are
    // computed by adding an offset to the source state rank (t = s + offset).
    // The distribution contains these offsets.
    Distribution<int> outcome_offsets;

public:
    explicit ProjectionOperator(OperatorID id);

    /// The operator ID of the task-level operator inducing this operator.
    OperatorID operator_id;

    friend bool are_equivalent(
        const ProjectionOperator& left,
        const ProjectionOperator& right)
    {
        return left.outcome_offsets == right.outcome_offsets;
    }
};

/**
 * @brief Helper class to convert projection operators to strings.
 */
class ProjectionOperatorToString {
    ProbabilisticTaskProxy task_proxy;

public:
    explicit ProjectionOperatorToString(ProbabilisticTaskProxy task_proxy);
    std::string operator()(const ProjectionOperator* op) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __ABSTRACT_OPERATOR_H__