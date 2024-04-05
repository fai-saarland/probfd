#include "probfd/pdbs/projection_transformation.h"

#include "probfd/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

namespace probfd::pdbs {

ProjectionTransformation::ProjectionTransformation(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    Pattern pattern,
    bool operator_pruning,
    double max_time)
    : ranking_function(task_proxy.get_variables(), std::move(pattern))
    , projection(std::make_unique<ProjectionStateSpace>(
          task_proxy,
          task_cost_function,
          ranking_function,
          operator_pruning,
          max_time))
    , distances(
          ranking_function.num_states(),
          std::numeric_limits<value_t>::quiet_NaN())
{
}

ProjectionTransformation::ProjectionTransformation(
    ProjectionTransformation&&) noexcept = default;
ProjectionTransformation& ProjectionTransformation::operator=(
    ProjectionTransformation&&) noexcept = default;
ProjectionTransformation::~ProjectionTransformation() = default;

}; // namespace probfd::pdbs