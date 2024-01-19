#include <utility>

#include "probfd/pdbs/pattern_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

PatternGenerator::PatternGenerator(const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
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
