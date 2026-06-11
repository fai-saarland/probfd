#include "downward_cli/pdbs/pattern_generator_manual_feature.h"

#include "downward_cli/pdbs/pattern_generator_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;

using namespace language;
using namespace language::plugins;

namespace {
class PatternGeneratorManualFeature : public TypedFeature<PatternGenerator> {
public:
    PatternGeneratorManualFeature()
        : TypedFeature("manual_pattern")
    {
        add_list_option<int>(
            "pattern",
            "list of variable numbers of the planning task that should be used "
            "as "
            "pattern.");
        add_generator_options_to_feature(*this);
    }

    shared_ptr<PatternGenerator>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorManual>(
            opts.get_list<int>(context, "pattern"),
            get_generator_arguments_from_options(context, opts));
    }
};
} // namespace

namespace downward::cli::pdbs {

void add_pattern_generator_manual_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorManualFeature>();
}

} // namespace downward::cli::pdbs
