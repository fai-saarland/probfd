#include "probfd/cli/pdbs/cegar/pucs_flaw_finder.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/cegar/pucs_flaw_finder.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

namespace {
class PUCSFlawFinderFeature
    : public TypedFeature<FlawFindingStrategy, PUCSFlawFinder> {
public:
    PUCSFlawFinderFeature()
        : TypedFeature("pucs_flaw_finder")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            Bounds("0", "infinity"));
    }

protected:
    std::shared_ptr<PUCSFlawFinder>
    create_component(const Options& options, const Context&) const override
    {
        return std::make_shared<PUCSFlawFinder>(
            options.get<int>("max_search_states"));
    }
};
}

namespace probfd::cli::pdbs::cegar {

void add_pucs_flaw_finder_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PUCSFlawFinderFeature>();
}

} // namespace
