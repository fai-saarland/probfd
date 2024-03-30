#ifndef PROBFD_PDBS_PROJECTION_OPERATOR_H
#define PROBFD_PDBS_PROJECTION_OPERATOR_H

#include "probfd/distribution.h"
#include "probfd/task_proxy.h"

#include "downward/operator_id.h"

#include <string>

/// Namespace dedicated to probabilistic pattern databases.
namespace probfd::pdbs {

/**
 * @brief Represents an operator of a projection state space.
 */
class ProjectionOperator {
    friend class ProjectionStateSpace;

public:
    /// The operator ID of the task-level operator inducing this operator.
    OperatorID operator_id;

private:
    // Implementation detail. Like in the classical implementation, each
    // operator is applicable in exactly one state. The successor states are
    // computed by adding an offset to the source state rank (t = s + offset).
    // The distribution contains these offsets.
    Distribution<int> outcome_offsets_;

public:
    explicit ProjectionOperator(OperatorID id, const auto& distr)
        : operator_id(id)
        , outcome_offsets_(distr)
    {
    }

    friend bool are_equivalent(
        const ProjectionOperator& left,
        const ProjectionOperator& right)
    {
        return left.outcome_offsets_ == right.outcome_offsets_;
    }
};

/**
 * @brief Helper class to convert projection operators to strings.
 */
class ProjectionOperatorToString {
    ProbabilisticTaskProxy task_proxy_;

public:
    explicit ProjectionOperatorToString(ProbabilisticTaskProxy task_proxy);
    std::string operator()(const ProjectionOperator* op) const;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_OPERATOR_H
