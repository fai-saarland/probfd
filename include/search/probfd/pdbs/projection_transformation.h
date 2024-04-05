#ifndef PROBFD_PDBS_PROJECTION_TRANSFORMATION_H
#define PROBFD_PDBS_PROJECTION_TRANSFORMATION_H

#include "probfd/pdbs/state_ranking_function.h"
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
    StateRankingFunction ranking_function;
    std::unique_ptr<ProjectionStateSpace> projection;
    std::vector<value_t> distances;

    // Constructs the transformation's abstraction mapping and state space,
    // and allocates the J* value table, initially filled with NaNs.
    ProjectionTransformation(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        Pattern pattern,
        bool operator_pruning = true,
        double max_time = std::numeric_limits<double>::infinity());

    ProjectionTransformation(ProjectionTransformation&&) noexcept;
    ProjectionTransformation& operator=(ProjectionTransformation&&) noexcept;
    ~ProjectionTransformation();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PROJECTION_TRANSFORMATION_H
