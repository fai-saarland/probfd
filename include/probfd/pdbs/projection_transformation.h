#ifndef PROBFD_PDBS_PROJECTION_TRANSFORMATION_H
#define PROBFD_PDBS_PROJECTION_TRANSFORMATION_H

#include "probfd/pdbs/probability_aware_pattern_database.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/value_type.h"

#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs {
class ProjectionStateSpace;
}

namespace probfd::pdbs {

struct ProjectionTransformation {
    ProbabilityAwarePatternDatabase pdb;
    std::unique_ptr<ProjectionStateSpace> projection;

    // Constructs the transformation's abstraction mapping and state space,
    // and allocates the J* value table, initially filled with NaNs.
    ProjectionTransformation(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        Pattern pattern,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    ProjectionTransformation(ProjectionTransformation&&) noexcept;
    ProjectionTransformation& operator=(ProjectionTransformation&&) noexcept;
    ~ProjectionTransformation();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_TRANSFORMATION_H
