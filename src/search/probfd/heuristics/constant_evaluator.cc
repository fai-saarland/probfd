#include "probfd/heuristics/constant_evaluator.h"

namespace probfd::heuristics {

std::unique_ptr<FDREvaluator> BlindEvaluatorFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<FDRCostFunction>)
{
    return std::make_unique<BlindEvaluator<State>>();
}

} // namespace probfd::heuristics
