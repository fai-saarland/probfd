#include "probfd/heuristics/determinization_cost_heuristic.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/heuristic.h"
#include "probfd/task_heuristic_factory.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

#include <utility>

using namespace downward;

namespace probfd::heuristics {

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    std::shared_ptr<Evaluator> evaluator)
    : evaluator_(std::move(evaluator))
{
}

DeterminizationCostHeuristic::~DeterminizationCostHeuristic() = default;

value_t DeterminizationCostHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    EvaluationResult result = evaluator_->compute_result(context);
    return result.is_infinite()
               ? INFINITE_VALUE
               : static_cast<value_t>(result.get_evaluator_value());
}

void DeterminizationCostHeuristic::print_statistics() const
{
    // evaluator_->print_statistics();
}

DeterminizationCostHeuristicFactory::DeterminizationCostHeuristicFactory(
    std::shared_ptr<TaskDependentFactory<Evaluator>> evaluator_factory)
    : evaluator_factory_(std::move(evaluator_factory))
{
}

std::unique_ptr<FDREvaluator>
DeterminizationCostHeuristicFactory::create_heuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction>)
{
    return std::make_unique<DeterminizationCostHeuristic>(
        evaluator_factory_->create_object(
            std::make_shared<tasks::DeterminizationTask>(task)));
}

} // namespace probfd::heuristics
