#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/task_evaluator_factory.h"

#include "probfd/fdr_types.h"

#include "downward/plugins/plugin.h"

#include <memory>

class State;

namespace plugins {
class Options;
}
namespace probfd {
class ProbabilisticTask;
}
namespace utils {
class Context;
}

namespace probfd {
namespace heuristics {

namespace {
class BlindEvaluatorFactory : public TaskEvaluatorFactory {
public:
    std::unique_ptr<FDREvaluator> create_evaluator(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction> task_cost_function) override;
};

std::unique_ptr<FDREvaluator> BlindEvaluatorFactory::create_evaluator(
    std::shared_ptr<ProbabilisticTask>,
    std::shared_ptr<FDRCostFunction>)
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
