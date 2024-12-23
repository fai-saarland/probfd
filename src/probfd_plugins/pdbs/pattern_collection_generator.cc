#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/utils/logging_options.h"

#include "probfd/pdbs/pattern_collection_generator.h"

using namespace probfd::pdbs;

using namespace downward_plugins::plugins;

using downward_plugins::utils::add_log_options_to_feature;
using downward_plugins::utils::get_log_arguments_from_options;

namespace probfd_plugins::pdbs {

void add_pattern_collection_generator_options_to_feature(Feature& feature)
{
    add_log_options_to_feature(feature);
}

std::tuple<utils::Verbosity>
get_collection_generator_arguments_from_options(const Options& opts)
{
    return get_log_arguments_from_options(opts);
}

} // namespace probfd_plugins::pdbs

namespace {

class PatternCollectionGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternCollectionGenerator> {
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
