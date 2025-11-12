#include "probfd/cli/pdbs/cegar/bfs_flaw_finder.h"

#include "downward/cli/plugins/registry.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

using namespace downward;
using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace {
class BFSFlawFinderFeature : public SharedTypedFeature<FlawFindingStrategy> {
public:
    BFSFlawFinderFeature()
        : TypedFeature("bfs_flaw_finder")
    {
        add_optional_argument_with_default<int>(
            "max_search_states",
            "20M",
            "Maximal number of generated states after which the flaw search is "
            "aborted.");
    }

protected:
    std::shared_ptr<FlawFindingStrategy>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<BFSFlawFinder>(
            options.get<int>("max_search_states"));
    }
};
} // namespace

namespace probfd::cli::pdbs::cegar {

void add_bfs_flaw_finder_feature(Registry& raw_registry)
{
    raw_registry.insert_feature_plugin<BFSFlawFinderFeature>();
}

} // namespace probfd::cli::pdbs::cegar
