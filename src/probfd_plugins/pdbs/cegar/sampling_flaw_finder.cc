#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

#include "downward/plugins/plugin.h"

#include "downward/utils/rng_options.h"

using namespace probfd::pdbs::cegar;

namespace {

class SamplingFlawFinderFeature
    : public plugins::TypedFeature<FlawFindingStrategy, SamplingFlawFinder> {
public:
    SamplingFlawFinderFeature()
        : TypedFeature("sampling_flaw_finder")
    {
        utils::add_rng_options_to_feature(*this);
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            plugins::Bounds("0", "infinity"));
    }

    std::shared_ptr<SamplingFlawFinder>
    create_component(const plugins::Options& opts, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<SamplingFlawFinder>(
            utils::get_rng(
                std::get<0>(utils::get_rng_arguments_from_options(opts))),
            opts.get<int>("max_search_states"));
    }
};

plugins::FeaturePlugin<SamplingFlawFinderFeature> _plugin;

} // namespace
