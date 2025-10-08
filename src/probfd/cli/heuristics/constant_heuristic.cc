#include "probfd/cli/heuristics/constant_heuristic.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/heuristics/constant_heuristic.h"

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::heuristics;

using namespace downward::cli::plugins;

namespace {
class BlindHeuristicFactoryFeature : public SharedTypedFeature<TaskHeuristicFactory> {
public:
    BlindHeuristicFactoryFeature()
        : SharedTypedFeature("blind_heuristic")
    {
        document_title("Blind Heuristic");
        document_synopsis(
            "This heuristic always returns an estimate of 0 for every state.");
    }

    [[nodiscard]]
    std::shared_ptr<TaskHeuristicFactory>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<BlindHeuristicFactory>();
    }
};
} // namespace

namespace probfd::cli::heuristics {

void add_blind_heuristic_factory_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<BlindHeuristicFactoryFeature>();
}

} // namespace probfd::cli::heuristics
