#include "downward/cli/pdbs/pattern_collection_generator_category.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "downward/pdbs/pattern_generator.h"

using namespace downward::pdbs;

using namespace downward::cli::plugins;

namespace {
class PatternCollectionGeneratorCategoryPlugin
    : public TypedCategoryPlugin<PatternCollectionGenerator> {
public:
    PatternCollectionGeneratorCategoryPlugin()
        : TypedCategoryPlugin("PatternCollectionGenerator")
    {
        document_synopsis("Factory for pattern collections");
    }
};
}

namespace downward::cli::pdbs {

void add_pattern_collection_generator_category(RawRegistry& raw_registry)
{
    raw_registry
        .insert_category_plugin<PatternCollectionGeneratorCategoryPlugin>();
}

} // namespace
