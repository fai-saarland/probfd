#include "downward/potentials/potential_max_heuristic.h"

#include "downward/potentials/potential_function.h"

#include "downward/task_transformation.h"

#include "downward/tasks/root_task.h"

using namespace std;

namespace downward::potentials {

PotentialMaxHeuristic::PotentialMaxHeuristic(
    vector<unique_ptr<PotentialFunction>>&& functions,
    std::shared_ptr<AbstractTask> original_task,
    const shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : PotentialMaxHeuristic(
          std::move(functions),
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

PotentialMaxHeuristic::PotentialMaxHeuristic(
    std::vector<std::unique_ptr<PotentialFunction>>&& functions,
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : PotentialMaxHeuristic(
          std::move(functions),
          std::move(original_task),
          std::move(transformation_result.transformed_task),
          std::move(transformation_result.state_mapping),
          std::move(transformation_result.inv_operator_mapping),
          cache_estimates,
          description,
          verbosity)

{
}

PotentialMaxHeuristic::PotentialMaxHeuristic(
    std::vector<std::unique_ptr<PotentialFunction>>&& functions,
    std::shared_ptr<AbstractTask> original_task,
    std::shared_ptr<AbstractTask> transformed_task,
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
    , functions(std::move(functions))
{
}

PotentialMaxHeuristic::~PotentialMaxHeuristic() = default;

int PotentialMaxHeuristic::compute_heuristic(const State& ancestor_state)
{
    State state = convert_ancestor_state(ancestor_state);
    int value = 0;
    for (auto& function : functions) {
        value = max(value, function->get_value(state));
    }
    return value;
}
} // namespace potentials
