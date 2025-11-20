#include "probfd/cli/pdbs/cegar/bfs_flaw_finder.h"

#include "downward/cli/plugins/registry.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/pdbs/cegar/bfs_flaw_finder.h"

using namespace downward;
using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace {
class BFSFlawFinderFeature
    : public SharedTypedFeature<FlawFindingStrategy, int> {
public:
    BFSFlawFinderFeature()
        : TypedFeature("bfs_flaw_finder", &BFSFlawFinderFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "max_search_states",
            "20M",
            "Maximal number of generated states after which the flaw search is "
            "aborted.");
    }

protected:
    static std::shared_ptr<FlawFindingStrategy> func(int max_search_states)
    {
        return std::make_shared<BFSFlawFinder>(max_search_states);
    }
};
} // namespace

namespace probfd::cli::pdbs::cegar {

void add_bfs_flaw_finder_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<BFSFlawFinderFeature>();
}

} // namespace probfd::cli::pdbs::cegar
