#include "downward/potentials/potential_heuristic.h"

#include "downward/potentials/potential_function.h"

#include "downward/task_transformation.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::potentials {
PotentialHeuristic::PotentialHeuristic(
    unique_ptr<PotentialFunction> function,
    SharedAbstractTask original_task,
    const shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : PotentialHeuristic(
          std::move(function),
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

PotentialHeuristic::PotentialHeuristic(
    std::unique_ptr<PotentialFunction> function,
    SharedAbstractTask original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : PotentialHeuristic(
          std::move(function),
          std::move(original_task),
          std::move(transformation_result.transformed_task),
          std::move(transformation_result.state_mapping),
          std::move(transformation_result.inv_operator_mapping),
          cache_estimates,
          description,
          verbosity)

{
}

PotentialHeuristic::PotentialHeuristic(
    std::unique_ptr<PotentialFunction> function,
    SharedAbstractTask original_task,
    SharedAbstractTask transformed_task,
    std::shared_ptr<StateMapping> state_mapping,
    std::shared_ptr<InverseOperatorMapping> inv_operator_mapping,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformed_task),
          std::move(state_mapping),
          std::move(inv_operator_mapping),
          cache_estimates,
          description,
          verbosity)
    , function(std::move(function))
{
}

PotentialHeuristic::~PotentialHeuristic() = default;

int PotentialHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    return max(0, function->get_value(state));
}
} // namespace potentials
