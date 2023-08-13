#include "probfd/heuristics/dead_end_pruning.h"

#include "probfd/cost_function.h"
#include "probfd/cost_model.h"

#include "downward/utils/system.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {

DeadEndPruningHeuristic::DeadEndPruningHeuristic(
    std::shared_ptr<::Evaluator> pruning_function,
    value_t dead_end_value)
    : pruning_function_(std::move(pruning_function))
    , dead_end_value_(dead_end_value)
{
    if (!pruning_function_->dead_ends_are_reliable()) {
        utils::g_log << "Dead end pruning heuristic was constructed with an "
                        "evaluator that has unreliable dead ends!"
                     << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

DeadEndPruningHeuristic::DeadEndPruningHeuristic(const plugins::Options& opts)
    : DeadEndPruningHeuristic(
          opts.get<std::shared_ptr<::Evaluator>>("heuristic"),
          g_cost_model->get_cost_function()->get_non_goal_termination_cost())
{
}

EvaluationResult DeadEndPruningHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    ::EvaluationResult result = pruning_function_->compute_result(context);

    if (result.is_infinite()) {
        return EvaluationResult(true, dead_end_value_);
    }
    return EvaluationResult(false, 0_vt);
}

void DeadEndPruningHeuristic::print_statistics() const
{
    // pruning_function_->print_statistics();
}

class DeadEndPruningHeuristicFeature
    : public plugins::TypedFeature<TaskEvaluator, DeadEndPruningHeuristic> {
public:
    DeadEndPruningHeuristicFeature()
        : TypedFeature("prune_dead_ends")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }
};

static plugins::FeaturePlugin<DeadEndPruningHeuristicFeature> _plugin;

} // namespace heuristics
} // namespace probfd
