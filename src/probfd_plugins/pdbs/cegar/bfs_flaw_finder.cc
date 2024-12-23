#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

#include "downward_plugins/plugins/plugin.h"

using namespace probfd::pdbs::cegar;

using namespace downward_plugins::plugins;

namespace {

class BFSFlawFinderFeature
    : public TypedFeature<FlawFindingStrategy, BFSFlawFinder> {
public:
    BFSFlawFinderFeature()
        : TypedFeature("bfs_flaw_finder")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            Bounds("0", "infinity"));
    }

protected:
    std::shared_ptr<BFSFlawFinder>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<BFSFlawFinder>(
            options.get<int>("max_search_states"));
    }
};

FeaturePlugin<BFSFlawFinderFeature> _plugin;

} // namespace
