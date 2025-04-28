#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"

using namespace downward;

namespace probfd::heuristics {

std::unique_ptr<FDRHeuristic>
BlindEvaluatorFactory::create_object(const SharedProbabilisticTask& task)
{
    return std::make_unique<BlindEvaluator<State>>(
        get_operators(task),
        get_cost_function(task),
        get_termination_costs(task));
}

} // namespace probfd::heuristics
