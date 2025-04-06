#include "downward/cli/plugins/plugin.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_combo.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;
using downward::cli::pdbs::get_generator_arguments_from_options;

namespace {

class PatternCollectionGeneratorComboFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorCombo> {
public:
    PatternCollectionGeneratorComboFeature()
        : TypedFeature("combo")
    {
        add_option<int>(
            "max_states",
            "maximum abstraction size for combo strategy",
            "1000000",
            Bounds("1", "infinity"));
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGeneratorCombo>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorCombo>(
            opts.get<int>("max_states"),
            get_generator_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorComboFeature> _plugin;

} // namespace
