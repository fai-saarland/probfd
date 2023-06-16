#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/cost_model.h"

#include "plugins/plugin.h"

#include <memory>

namespace probfd {
namespace heuristics {

class ConstantEvaluatorFeature
    : public plugins::TypedFeature<TaskEvaluator, ConstantEvaluator<State>> {
public:
    ConstantEvaluatorFeature()
        : TypedFeature("const_eval")
    {
        add_option<double>("value", "", plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<ConstantEvaluator<State>>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return std::make_shared<ConstantEvaluator<State>>(
            double_to_value(opts.get<double>("value")));
    }
};

class BlindEvaluatorFeature
    : public plugins::TypedFeature<TaskEvaluator, BlindEvaluator<State>> {
public:
    BlindEvaluatorFeature()
        : TypedFeature("blind_eval")
    {
    }

    std::shared_ptr<BlindEvaluator<State>>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<BlindEvaluator<State>>();
    }
};

static plugins::FeaturePlugin<ConstantEvaluatorFeature> _plugin;
static plugins::FeaturePlugin<BlindEvaluatorFeature> _plugin2;

} // namespace heuristics
} // namespace probfd
