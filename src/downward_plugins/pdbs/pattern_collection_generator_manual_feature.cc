#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/pdbs/pattern_generator_options.h"

#include "downward/pdbs/pattern_collection_generator_manual.h"

using namespace std;
using namespace utils;
using namespace pdbs;

using namespace downward_plugins::pdbs;
using namespace downward_plugins::plugins;

namespace {

class PatternCollectionGeneratorManualFeature
    : public TypedFeature<
          PatternCollectionGenerator,
          PatternCollectionGeneratorManual> {
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

    virtual shared_ptr<PatternCollectionGeneratorManual>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<PatternCollectionGeneratorManual>(
            opts.get_list<Pattern>("patterns"),
            get_generator_arguments_from_options(opts));
    }
};

FeaturePlugin<PatternCollectionGeneratorManualFeature> _plugin;

} // namespace
