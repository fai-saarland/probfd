#include "probfd/heuristics/determinization_cost.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {

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

namespace {
class DeterminizationCostHeuristicFactory : public TaskEvaluatorFactory {
    const std::shared_ptr<::Evaluator> evaluator_;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - Only one option is available:
     * + heuristic - Specifies the underlying classical heuristic.
     */
    explicit DeterminizationCostHeuristicFactory(const plugins::Options& opts);

    std::unique_ptr<TaskEvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<TaskCostFunction> task_cost_function) override;
};

DeterminizationCostHeuristicFactory::DeterminizationCostHeuristicFactory(
    const plugins::Options& opts)
    : evaluator_(opts.get<std::shared_ptr<::Evaluator>>("evaluator"))
{
}

std::unique_ptr<TaskEvaluator>
DeterminizationCostHeuristicFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<TaskCostFunction>)
{
    return std::make_unique<DeterminizationCostHeuristic>(evaluator_);
}

class DeterminizationHeuristicFactoryFeature
    : public plugins::TypedFeature<
          TaskEvaluatorFactory,
          DeterminizationCostHeuristicFactory> {
public:
    DeterminizationHeuristicFactoryFeature()
        : TypedFeature("det")
    {
        add_option<std::shared_ptr<::Evaluator>>("evaluator");
    }
};

static plugins::FeaturePlugin<DeterminizationHeuristicFactoryFeature> _plugin;
} // namespace

} // namespace heuristics
} // namespace probfd
