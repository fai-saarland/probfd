#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/rng_options.h"

#include "probfd/pdbs/cegar/sampling_flaw_finder.h"

#include "downward/utils/rng_options.h"

using namespace downward;
using namespace utils;

using namespace probfd::pdbs::cegar;

using namespace downward::cli::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {

class SamplingFlawFinderFeature
    : public TypedFeature<FlawFindingStrategy, SamplingFlawFinder> {
public:
    SamplingFlawFinderFeature()
        : TypedFeature("sampling_flaw_finder")
    {
        add_rng_options_to_feature(*this);
        add_option<int>(
            "max_search_states",
            "Maximal number of generated states after which the flaw search is "
            "aborted.",
            "20M",
            Bounds("0", "infinity"));
    }

    std::shared_ptr<SamplingFlawFinder>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<SamplingFlawFinder>(
            get_rng(std::get<0>(get_rng_arguments_from_options(opts))),
            opts.get<int>("max_search_states"));
    }
};

FeaturePlugin<SamplingFlawFinderFeature> _plugin;

} // namespace
