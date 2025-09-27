#include "probfd/heuristics/dead_end_pruning_heuristic.h"

#include "probfd/tasks/determinization_task.h"

#include "probfd/heuristic.h"
#include "probfd/probabilistic_task.h"
#include "probfd/task_heuristic_factory.h"
#include "probfd/termination_costs.h"

#include "downward/utils/system.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"

#include <iostream>
#include <utility>

using namespace downward;

namespace probfd::heuristics {

DeadEndPruningHeuristic::DeadEndPruningHeuristic(
    std::shared_ptr<Evaluator> pruning_function,
    value_t dead_end_value)
    : pruning_function_(std::move(pruning_function))
    , dead_end_value_(dead_end_value)
{
    if (!pruning_function_->dead_ends_are_reliable()) {
        std::println(
            std::cerr,
            "Dead end pruning heuristic was constructed with an "
            "evaluator that has unreliable dead ends!");
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

value_t DeadEndPruningHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    EvaluationResult result = pruning_function_->compute_result(context);
    return result.is_infinite() ? dead_end_value_ : 0_vt;
}

void DeadEndPruningHeuristic::print_statistics() const
{
    // pruning_function_->print_statistics();
}

DeadEndPruningHeuristicFactory::DeadEndPruningHeuristicFactory(
    std::shared_ptr<downward::TaskDependentFactory<Evaluator>>
        evaluator_factory)
    : evaluator_factory_(std::move(evaluator_factory))
{
}

std::unique_ptr<FDRHeuristic> DeadEndPruningHeuristicFactory::create_object(
    const SharedProbabilisticTask& task)
{
    return std::make_unique<DeadEndPruningHeuristic>(
        evaluator_factory_->create_object(
            tasks::create_determinization_task(task)),
        get_shared_termination_costs(task)->get_non_goal_termination_cost());
}

} // namespace probfd::heuristics
