#include "probfd/heuristics/pdbs/pattern_collection_generator.h"

#include "probfd/heuristics/pdbs/pdb_collection_information.h"
#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternCollectionGenerator::PatternCollectionGenerator(
    const plugins::Options& opts)
    : log(utils::get_log_from_options(opts))
{
}

PatternCollectionGenerator::PatternCollectionGenerator(utils::LogProxy log)
    : log(std::move(log))
{
}

void add_pattern_collection_generator_options_to_feature(
    plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

namespace {
static class PatternCollectionGeneratorCategoryPlugin
    : public plugins::TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("ProbFDPatternCollectionGenerator")
    {
        document_synopsis("Factory for patterns");
    }
} _category_plugin;
} // namespace

} // namespace pdbs
} // namespace heuristics
} // namespace probfd