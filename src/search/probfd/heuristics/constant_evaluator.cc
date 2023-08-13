#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/task_evaluator_factory.h"

#include "downward/plugins/plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {

namespace {
class BlindEvaluatorFactory : public TaskEvaluatorFactory {
public:
    std::unique_ptr<TaskEvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<TaskCostFunction> task_cost_function) override;
};

std::unique_ptr<TaskEvaluator> BlindEvaluatorFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<TaskCostFunction>)
{
    return std::make_unique<BlindEvaluator<State>>();
}

class BlindEvaluatorFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, BlindEvaluatorFactory> {
public:
    BlindEvaluatorFactoryFeature()
        : TypedFeature("blind_eval")
    {
    }

    std::shared_ptr<BlindEvaluatorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<BlindEvaluatorFactory>();
    }
};

static plugins::FeaturePlugin<BlindEvaluatorFactoryFeature> _plugin;

} // namespace

} // namespace heuristics
} // namespace probfd
