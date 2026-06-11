#include "probfd_cli/pdbs/cegar/pucs_flaw_finder.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd/pdbs/cegar/pucs_flaw_finder.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace language;
using namespace language::plugins;

namespace {
class PUCSFlawFinderFeature : public TypedFeature<FlawFindingStrategy> {
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
    std::shared_ptr<FlawFindingStrategy>
    create_component(const Options& options, const Context& context)
        const override
    {
        return std::make_shared<PUCSFlawFinder>(
            options.get<int>(context, "max_search_states"));
    }
};
} // namespace

namespace probfd::cli::pdbs::cegar {

void add_pucs_flaw_finder_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PUCSFlawFinderFeature>();
}

} // namespace probfd::cli::pdbs::cegar
