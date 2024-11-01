#include "probfd/pdbs/pattern_collection_generator.h"

#include "downward/plugins/plugin.h"

using namespace probfd::pdbs;

namespace probfd_plugins::pdbs {

void add_pattern_collection_generator_options_to_feature(
    plugins::Feature& feature)
{
    utils::add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_collection_generator_arguments_from_options(const plugins::Options& opts)
{
    return utils::get_log_arguments_from_options(opts);
}

} // namespace probfd_plugins::pdbs

namespace {

class PatternCollectionGeneratorCategoryPlugin
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

} // namespace
