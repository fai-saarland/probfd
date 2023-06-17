#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionGenerator::PatternCollectionGenerator(
    const plugins::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PatternCollectionGenerator::PatternCollectionGenerator(
    const utils::LogProxy& log)
    : log(log)
{
}

PatternGenerator::PatternGenerator(const plugins::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PatternGenerator::PatternGenerator(const utils::LogProxy& log)
    : log(log)
{
}

void add_generator_options_to_feature(plugins::Feature& feature)
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
} _category_plugin_collection;

static class PatternGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternGenerator> {
public:
    PatternGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternGenerator")
    {
        document_synopsis("Factory for a pattern and/or the corresponding "
                          "probability-aware PDB");
    }
} _category_plugin_single;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd