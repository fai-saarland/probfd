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

namespace {
class PatternCollectionGeneratorComboFeature
    : public SharedTypedFeature<
          PatternCollectionGenerator,
          int,
          downward::utils::Verbosity> {
public:
    PatternCollectionGeneratorComboFeature()
        : TypedFeature("combo", &PatternCollectionGeneratorComboFeature::func)
    {
        make_optional_argument_with_default(
            0,
            "max_states",
            "1000000",
            "maximum abstraction size for combo strategy");
        add_generator_options_to_feature(*this, 1);
    }

    static shared_ptr<PatternCollectionGenerator>
    func(const Context&, int max_states, downward::utils::Verbosity verbosity)
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorCombo>(
            max_states,
            verbosity);
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_combo_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<PatternCollectionGeneratorComboFeature>();
}

} // namespace downward::cli::pdbs
