#include "probfd/heuristics/constant_evaluator.h"

#include "downward/plugins/plugin.h"

using namespace probfd;
using namespace probfd::heuristics;

namespace {

class BlindEvaluatorFactoryFeature
    : public plugins::
          TypedFeature<TaskEvaluatorFactory, BlindEvaluatorFactory> {
public:
    BlindEvaluatorFactoryFeature()
        : TypedFeature("blind_eval")
    {
    }

    [[nodiscard]]
    std::shared_ptr<BlindEvaluatorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<BlindEvaluatorFactory>();
    }
};

plugins::FeaturePlugin<BlindEvaluatorFactoryFeature> _plugin;

} // namespace
