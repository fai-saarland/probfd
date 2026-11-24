#ifndef PROBFD_PDBS_PROJECTION_OPERATOR_H
#define PROBFD_PDBS_PROJECTION_OPERATOR_H

#include "probfd/pmf.h"

#include "downward/operator_id.h"

#include <ranges>
#include <string>

namespace probfd {
class ProbabilisticOperatorSpace;
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
    // The PMF contains these offsets.
    PMF<int> outcome_offset_pmf_;

public:
    template <std::ranges::input_range R>
        requires(std::convertible_to<
                    std::ranges::range_reference_t<R>,
                    ItemProbabilityPair<int>>)
    explicit ProjectionOperator(downward::OperatorID id, R&& pmf)
        : operator_id(id)
        , outcome_offset_pmf_(std::from_range, std::forward<R>(pmf))
    {
    }

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                    std::ranges::range_reference_t<R>,
                    ItemProbabilityPair<int>>)
    explicit ProjectionOperator(
        downward::OperatorID id,
        R&& pmf,
        NoNormalizeTagType)
        : operator_id(id)
        , outcome_offset_pmf_(
              std::from_range,
              no_normalize,
              std::forward<R>(pmf))
    {
    }

    friend bool are_equivalent(
        const ProjectionOperator& left,
        const ProjectionOperator& right)
    {
        return left.outcome_offset_pmf_ == right.outcome_offset_pmf_;
    }
};

/**
 * @brief Helper class to convert projection operators to strings.
 */
class ProjectionOperatorToString {
    const ProbabilisticOperatorSpace& operators_;

public:
    explicit ProjectionOperatorToString(const ProbabilisticOperatorSpace& operators);
    std::string operator()(const ProjectionOperator* op) const;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_OPERATOR_H
