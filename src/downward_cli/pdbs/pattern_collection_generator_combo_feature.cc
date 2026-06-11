#include "downward_cli/pdbs/pattern_collection_generator_combo_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_combo.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace language;
using namespace language::plugins;

using downward::cli::pdbs::add_generator_options_to_feature;
using downward::cli::pdbs::get_generator_arguments_from_options;

namespace {
class PatternCollectionGeneratorComboFeature
    : public TypedFeature<PatternCollectionGenerator> {
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

    shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorCombo>(
            opts.get<int>(context, "max_states"),
            get_generator_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_combo_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionGeneratorComboFeature>();
}

} // namespace downward::cli::pdbs
