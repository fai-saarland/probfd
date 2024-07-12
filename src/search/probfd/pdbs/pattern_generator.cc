#include <utility>

#include "probfd/pdbs/pattern_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

PatternGenerator::PatternGenerator(utils::Verbosity verbosity)
    : log_(utils::get_log_for_verbosity(verbosity))
{
}

PatternGenerator::PatternGenerator(utils::LogProxy log)
    : log_(std::move(log))
{
}

void add_pattern_generator_options_to_feature(plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_generator_arguments_from_options(const plugins::Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

static class PatternGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternGenerator")
    {
        document_synopsis("Factory for a pattern and/or the corresponding "
                          "probability-aware PDB");
    }
} _category_plugin;

} // namespace probfd::pdbs
