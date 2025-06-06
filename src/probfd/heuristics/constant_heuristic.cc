#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/probabilistic_task.h"

using namespace downward;

namespace probfd::heuristics {

std::unique_ptr<FDREvaluator> BlindEvaluatorFactory::create_heuristic(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> cost_function)
{
    ProbabilisticOperatorsProxy ops(*task);
    return std::make_unique<BlindEvaluator<State>>(ops, *cost_function);
}

} // namespace probfd::heuristics
