#include "probfd/pdbs/cegar/pucs_flaw_finder.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs::cegar;

namespace {

class PUCSFlawFinderFeature
    : public plugins::TypedFeature<FlawFindingStrategy, PUCSFlawFinder> {
public:
    PUCSFlawFinderFeature()
        : TypedFeature("pucs_flaw_finder")
    {
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }

protected:
    std::shared_ptr<PUCSFlawFinder>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        return std::make_shared<PUCSFlawFinder>(
            options.get<int>("max_search_states"));
    }
};

plugins::FeaturePlugin<PUCSFlawFinderFeature> _plugin;

} // namespace
