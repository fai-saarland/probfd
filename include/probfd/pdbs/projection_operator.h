#ifndef PROBFD_PDBS_PROJECTION_OPERATOR_H
#define PROBFD_PDBS_PROJECTION_OPERATOR_H

#include "probfd/distribution.h"

#include "downward/operator_id.h"

#include <ranges>
#include <string>

namespace probfd {
class ProbabilisticTask;
}

/// Namespace dedicated to probabilistic pattern databases.
namespace probfd::pdbs {

/**
 * @brief Represents an operator of a projection state space.
 */
class ProjectionOperator {
    friend class ProjectionStateSpace;

public:
    /// The operator ID of the task-level operator inducing this operator.
    downward::OperatorID operator_id;

private:
    // Implementation detail. Like in the classical implementation, each
    // operator is applicable in exactly one state. The successor states are
    // computed by adding an offset to the source state rank (t = s + offset).
    // The distribution contains these offsets.
    Distribution<int> outcome_offsets_;

public:
    template <std::ranges::input_range R>
        requires(std::convertible_to<
                    std::ranges::range_reference_t<R>,
                    ItemProbabilityPair<int>>)
    explicit ProjectionOperator(downward::OperatorID id, R&& distr)
        : operator_id(id)
        , outcome_offsets_(std::from_range, std::forward<R>(distr))
    {
    }

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                    std::ranges::range_reference_t<R>,
                    ItemProbabilityPair<int>>)
    explicit ProjectionOperator(
        downward::OperatorID id,
        R&& distr,
        NoNormalizeTagType)
        : operator_id(id)
        , outcome_offsets_(
              std::from_range,
              no_normalize,
              std::forward<R>(distr))
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
    const ProbabilisticTask& task_;

public:
    explicit ProjectionOperatorToString(const ProbabilisticTask& task);
    std::string operator()(const ProjectionOperator* op) const;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_OPERATOR_H
