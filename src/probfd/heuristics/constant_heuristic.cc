#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/probabilistic_task.h"

using namespace downward;

namespace probfd::heuristics {

std::unique_ptr<FDREvaluator>
BlindEvaluatorFactory::create_heuristic(std::shared_ptr<ProbabilisticTask> task)
{
    return std::make_unique<BlindEvaluator<State>>(
        task->get_operators(),
        *task,
        *task);
}

} // namespace probfd::heuristics
