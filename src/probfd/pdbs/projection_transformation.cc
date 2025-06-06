#include "probfd/pdbs/projection_transformation.h"

#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

namespace probfd::pdbs {

ProjectionTransformation::ProjectionTransformation(
    ProbabilisticTaskProxy task_proxy,
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
    Pattern pattern,
    bool operator_pruning,
    double max_time)
    : pdb(task_proxy.get_variables(), std::move(pattern))
    , projection(std::make_unique<ProjectionStateSpace>(
          task_proxy,
          std::move(task_cost_function),
          pdb.ranking_function,
          operator_pruning,
          max_time))
{
}

ProjectionTransformation::ProjectionTransformation(
    ProjectionTransformation&&) noexcept = default;
ProjectionTransformation& ProjectionTransformation::operator=(
    ProjectionTransformation&&) noexcept = default;
ProjectionTransformation::~ProjectionTransformation() = default;

}; // namespace probfd::pdbs