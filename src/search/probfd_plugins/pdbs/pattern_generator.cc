#include "probfd_plugins/pdbs/pattern_generator.h"

#include "probfd/pdbs/pattern_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace probfd_plugins::pdbs {

void add_pattern_generator_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_generator_arguments_from_options(const plugins::Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

} // namespace probfd_plugins::pdbs

namespace {

class PatternGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternGenerator")
    {
        document_synopsis("Factory for a pattern and/or the corresponding "
                          "probability-aware PDB");
    }
} _category_plugin;

} // namespace
