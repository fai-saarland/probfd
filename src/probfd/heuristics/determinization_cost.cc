#include "probfd/heuristics/determinization_cost.h"

#include "probfd/evaluator.h"
#include "probfd/task_evaluator_factory.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"

#include <utility>

namespace probfd::heuristics {

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    std::shared_ptr<::Evaluator> evaluator)
    : evaluator_(std::move(evaluator))
{
}

DeterminizationCostHeuristic::~DeterminizationCostHeuristic() = default;

value_t DeterminizationCostHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    ::EvaluationResult result = evaluator_->compute_result(context);
    return result.is_infinite()
               ? INFINITE_VALUE
               : static_cast<value_t>(result.get_evaluator_value());
}

void DeterminizationCostHeuristic::print_statistics() const
{
    // evaluator_->print_statistics();
}

DeterminizationCostHeuristicFactory::DeterminizationCostHeuristicFactory(
    std::shared_ptr<::Evaluator> evaluator)
    : evaluator_(std::move(evaluator))
{
}

std::unique_ptr<FDREvaluator>
DeterminizationCostHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<FDRCostFunction>)
{
    return std::make_unique<DeterminizationCostHeuristic>(evaluator_);
}

} // namespace probfd::heuristics
