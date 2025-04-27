#include "probfd/pdbs/projection_transformation.h"

#include "probfd/pdbs/projection_state_space.h"

#include "probfd/probabilistic_task.h"

namespace probfd::pdbs {

ProjectionTransformation::ProjectionTransformation(
    SharedProbabilisticTask task,
    Pattern pattern,
    bool operator_pruning,
    double max_time)
    : pdb(get_variables(task), std::move(pattern))
    , projection(std::make_unique<ProjectionStateSpace>(
          std::move(task),
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