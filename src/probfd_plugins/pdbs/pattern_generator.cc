#include "probfd_plugins/pdbs/pattern_generator.h"

#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/utils/logging_options.h"

#include "probfd/pdbs/pattern_generator.h"

using namespace probfd::pdbs;

using namespace downward_plugins::plugins;

using downward_plugins::utils::add_log_options_to_feature;
using downward_plugins::utils::get_log_arguments_from_options;

namespace probfd_plugins::pdbs {

void add_pattern_generator_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_generator_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace probfd_plugins::pdbs

namespace {

class PatternGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternGenerator")
    {
        document_synopsis("Factory for a pattern and/or the corresponding "
                          "probability-aware PDB");
    }
} _category_plugin;

} // namespace
