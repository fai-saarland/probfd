#include "probfd/cli/pdbs/pattern_collection_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/pdbs/pattern_collection_generator.h"

using namespace probfd::pdbs;

using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PPDBPatternCollectionGenerator")
    {
        document_synopsis(
            "Factory for pattern collections and/or "
            "corresponding probability-aware "
            "PDBs");
    }
};
}

namespace probfd::cli::pdbs {

void add_pattern_collection_generator_category(RawRegistry& raw_registry)
{
    raw_registry
        .insert_category_plugin<PatternCollectionGeneratorCategoryPlugin>();
}

} // namespace
