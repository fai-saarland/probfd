#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "probfd/heuristics/pdbs/pattern_collection_information.h"

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

} // namespace pdbs
} // namespace heuristics
} // namespace probfd