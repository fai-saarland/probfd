#include "downward/cli/plugins/plugin.h"

#include "probfd/heuristics/constant_evaluator.h"

using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {

class BlindEvaluatorFactoryFeature
    : public TypedFeature<TaskEvaluatorFactory, BlindEvaluatorFactory> {
public:
    BlindEvaluatorFactoryFeature()
        : TypedFeature("blind_heuristic")
    {
        document_title("Blind Heuristic");
        document_synopsis(
            "This heuristic always returns an estimate of 0 for every state.");
    }

    [[nodiscard]]
    std::shared_ptr<BlindEvaluatorFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<BlindEvaluatorFactory>();
    }
};

FeaturePlugin<BlindEvaluatorFactoryFeature> _plugin;

} // namespace
