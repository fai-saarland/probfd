#include "downward/cli/pdbs/pattern_generator_manual_feature.h"

#include "downward/cli/pdbs/pattern_generator_options.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace downward::utils;
using namespace downward::pdbs;

using namespace downward::cli::pdbs;
using namespace downward::cli::plugins;

namespace {
class PatternGeneratorManualFeature
    : public TypedFeature<PatternGenerator, PatternGeneratorManual> {
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

    virtual shared_ptr<PatternGeneratorManual>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternGeneratorManual>(
            opts.get_list<int>("pattern"),
            get_generator_arguments_from_options(opts));
    }
};
}

namespace downward::cli::pdbs {

void add_pattern_generator_manual_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<PatternGeneratorManualFeature>();
}

} // namespace
