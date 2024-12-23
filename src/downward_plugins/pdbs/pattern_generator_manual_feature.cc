#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_generator_manual.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::pdbs;
using namespace downward_plugins::plugins;

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

FeaturePlugin<PatternGeneratorManualFeature> _plugin;

} // namespace
