#include "downward_cli/pdbs/pattern_collection_generator_manual_feature.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward_cli/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace language;
using namespace language::plugins;

namespace {
class PatternCollectionGeneratorManualFeature
    : public TypedFeature<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorManualFeature()
        : TypedFeature("manual_patterns")
    {
        add_list_option<Pattern>(
            "patterns",
            "list of patterns (which are lists of variable numbers of the "
            "planning "
            "task).");
        add_generator_options_to_feature(*this);
    }

    shared_ptr<PatternCollectionGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorManual>(
            opts.get_list<Pattern>(context, "patterns"),
            get_generator_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_collection_generator_manual_feature(RawRegistry& raw_registry)
{
    raw_registry
        .insert_feature_plugin<PatternCollectionGeneratorManualFeature>();
}

} // namespace downward::cli::pdbs
