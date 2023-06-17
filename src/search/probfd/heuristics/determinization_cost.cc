#include "probfd/heuristics/determinization_cost.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    const plugins::Options& opts)
    : DeterminizationCostHeuristic(
          opts.get<std::shared_ptr<::Evaluator>>("evaluator"))
{
}

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    std::shared_ptr<::Evaluator> evaluator)
    : evaluator_(std::move(evaluator))
{
}

DeterminizationCostHeuristic::~DeterminizationCostHeuristic() = default;

EvaluationResult
DeterminizationCostHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    ::EvaluationResult result = evaluator_->compute_result(context);

    const bool infinite = result.is_infinite();
    return EvaluationResult(
        infinite,
        infinite ? INFINITE_VALUE : result.get_evaluator_value());
}

void DeterminizationCostHeuristic::print_statistics() const
{
    // evaluator_->print_statistics();
}

class DeterminizationHeuristicFeature
    : public plugins::
          TypedFeature<TaskEvaluator, DeterminizationCostHeuristic> {
public:
    DeterminizationHeuristicFeature()
        : TypedFeature("det")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }
};

static plugins::FeaturePlugin<DeterminizationHeuristicFeature> _plugin;

} // namespace heuristics
} // namespace probfd
