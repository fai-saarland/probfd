#include "downward/cli/pdbs/pattern_collection_generator_combo_feature.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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
    : public SharedTypedFeature<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorComboFeature()
        : TypedFeature("combo")
    {
        add_optional_argument_with_default<int>(
            "max_states",
            "1000000",
            "maximum abstraction size for combo strategy");
        add_generator_options_to_feature(*this);
    }

    virtual shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorCombo>(
            opts.get<int>("max_states"),
            get_generator_arguments_from_options(opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_combo_feature(Registry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionGeneratorComboFeature>();
}

} // namespace downward::cli::pdbs
