#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/probabilistic_task.h"

namespace probfd::heuristics {

std::unique_ptr<FDREvaluator> BlindEvaluatorFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> cost_function)
{
    ProbabilisticOperatorsProxy ops(*task);
    return std::make_unique<BlindEvaluator<State>>(ops, *cost_function);
}

} // namespace probfd::heuristics
