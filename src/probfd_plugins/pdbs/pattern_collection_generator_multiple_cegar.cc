#include "downward_plugins/plugins/plugin.h"

#include "probfd_plugins/pdbs/cegar_options.h"
#include "probfd_plugins/pdbs/pattern_collection_generator_multiple.h"

#include "probfd/pdbs/pattern_collection_generator_multiple_cegar.h"

using namespace utils;

using namespace probfd_plugins::pdbs;

using namespace probfd::pdbs;
using namespace probfd::pdbs::cegar;

using namespace downward_plugins::plugins;

namespace {

class PatternCollectionGeneratorMultipleCegarFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorMultipleCegar> {
public:
    PatternCollectionGeneratorMultipleCegarFeature()
        : TypedFeature("ppdbs_multiple_cegar")
    {
        add_option<std::shared_ptr<cegar::FlawFindingStrategy>>(
            "flaw_strategy",
            "strategy used to find flaws in a policy",
            "pucs_flaw_finder()");
        add_cegar_wildcard_option_to_feature(*this);
        add_multiple_options_to_feature(*this);
    }

    std::shared_ptr<PatternCollectionGeneratorMultipleCegar>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<
            PatternCollectionGeneratorMultipleCegar>(
            opts.get<std::shared_ptr<cegar::FlawFindingStrategy>>(
                "flaw_strategy"),
            get_cegar_wildcard_arguments_from_options(opts),
            get_multiple_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorMultipleCegarFeature> _plugin;

} // namespace
