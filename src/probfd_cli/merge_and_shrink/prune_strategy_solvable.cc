#include "probfd_cli/merge_and_shrink/prune_strategy_solvable.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

using namespace probfd::merge_and_shrink;
using namespace downward;

using namespace language;
using namespace language::plugins;

namespace {
class PruneStrategySolvableFeature : public TypedFeature<PruneStrategy> {
public:
    PruneStrategySolvableFeature()
        : TypedFeature("prune_solvable")
    {
        document_title("Solvable states prune strategy");
        document_synopsis("This prune strategy keeps only solvable states.");
    }

    std::shared_ptr<PruneStrategy>
    create_component(const Options&, const Context&) const override
    {
        return std::make_shared<PruneStrategySolvable>();
    }
};
} // namespace

namespace probfd::cli::merge_and_shrink {

void add_prune_strategy_solvable_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PruneStrategySolvableFeature>();
}

} // namespace probfd::cli::merge_and_shrink