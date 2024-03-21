#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

#include "downward/cli/plugins/plugin.h"

using namespace probfd::merge_and_shrink;
using namespace downward::cli::plugins;

namespace {

class PruneStrategySolvableFeature
    : public TypedFeature<PruneStrategy, PruneStrategySolvable> {
public:
    PruneStrategySolvableFeature()
        : TypedFeature("prune_solvable")
    {
        document_title("Solvable states prune strategy");
        document_synopsis("This prune strategy keeps only solvable states.");
    }

    std::shared_ptr<PruneStrategySolvable>
    create_component(const Options&, const utils::Context&)
        const override
    {
        return std::make_shared<PruneStrategySolvable>();
    }
} _plugin;

} // namespace probfd::merge_and_shrink