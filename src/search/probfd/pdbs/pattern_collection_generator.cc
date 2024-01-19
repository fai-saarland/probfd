#include <utility>

#include "probfd/pdbs/pattern_collection_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd::pdbs {

PatternCollectionGenerator::PatternCollectionGenerator(
    const plugins::Options& opts)
    : log_(utils::get_log_from_options(opts))
{
}

PatternCollectionGenerator::PatternCollectionGenerator(utils::LogProxy log)
    : log_(std::move(log))
{
}

void add_pattern_collection_generator_options_to_feature(
    plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

static class PatternCollectionGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternCollectionGenerator")
    {
        document_synopsis("Factory for pattern collections and/or "
                          "corresponding probability-aware "
                          "PDBs");
    }
} _category_plugin;

} // namespace probfd::pdbs
